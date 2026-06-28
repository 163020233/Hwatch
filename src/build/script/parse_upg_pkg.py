#!/usr/bin/env python3
# coding=utf-8

import struct
import os
import sys
import subprocess
import platform
from ctypes import *

td_char = c_char
td_s8 = c_byte
td_u8 = c_ubyte
td_s16 = c_short
td_u16 = c_ushort
td_s32 = c_int
td_u32 = c_uint

LZMA_COMPRESS_FLAG = 0x3C7896E1

fota_key_area_data_fields = [
    ("image_id", td_u32),
    ("struct_version", td_u32),
    ("struct_length", td_u32),
    ("signature_length", td_u32),
    ("key_owner_id", td_u32),
    ("key_id", td_u32),
    ("key_alg", td_u32),
    ("ecc_curve_type", td_u32),
    ("key_length", td_u32),
    ("fota_key_version_ext", td_u32),
    ("mask_fota_key_version_ext", td_u32),
    ("msid_ext", td_u32),
    ("mask_msid_ext", td_u32),
    ("maintenance_mode", td_u32),
    ("die_id", td_u8 * 16),
    ("fota_info_addr", td_u32),
    ("reserved", td_u8 * 176),
    ("fota_external_public_key", td_u8 * 516),
    ("sig_fota_key_area", td_u8 * 512),
]

fota_info_area_data_fields = [
    ("image_id", td_u32),
    ("struct_version", td_u32),
    ("struct_length", td_u32),
    ("signature_length", td_u32),
    ("fota_version_ext", td_u32),
    ("mask_fota_version_ext", td_u32),
    ("msid_ext", td_u32),
    ("mask_msid_ext", td_u32),
    ("image_hash_table_addr", td_u32),
    ("image_hash_table_length", td_u32),
    ("image_hash_table_hash", td_u8 * 32),
    ("image_num", td_u32),
    ("hardware_id", td_u32),
    ("reserved", td_u8 * 176),
    ("sign_fota_info", td_u8 * 512),
]

class fota_key_area_data(Structure):
    pass

class fota_info_area_data(Structure):
    pass

class fota_image_hash_node(Structure):
    _fields_ = [
        ("image_id", td_u32),
        ("image_addr", td_u32),
        ("image_length", td_u32),
        ("image_hash", td_u8 * 32),
    ]

class fota_image_head(Structure):
    _fields_ = [
        ("header_magic", td_u32),
        ("image_id", td_u32),
        ("image_offset", td_u32),
        ("image_len", td_u32),
        ("image_hash", td_u8 * 32),
        ("old_image_len", td_u32),
        ("old_image_hash", td_u8 * 32),
        ("new_image_len", td_u32),
        ("version_ext", td_u32),
        ("version_mask", td_u32),
        ("decompress_flag", td_u32),
        ("re_enc_flag", td_u32),
        ("root_key_type", td_u32),
        ("enc_pk_l1", td_u8 * 16),
        ("enc_pk_l2", td_u8 * 16),
        ("iv", td_u8 * 16),
        ("padding", td_u8 * 4),
    ]

def find_lzma_tool():
    script_dir = os.path.dirname(os.path.realpath(__file__))

    search_paths = [
        script_dir,
        os.path.normpath(os.path.join(script_dir, "..", "..", "tools", "bin", "lzma_tool")),
    ]

    if platform.system().lower() == "windows":
        tool_name = "lzma_tool.exe"
    else:
        tool_name = "lzma_tool"

    for search_dir in search_paths:
        tool_path = os.path.join(search_dir, tool_name)
        if os.path.exists(tool_path):
            return tool_path

    return None

def decompress_lzma(src_file, dst_file):
    tool_path = find_lzma_tool()

    if tool_path is None:
        print(f"  WARNING: LZMA tool not found in script directory or tools/bin/lzma_tool/")
        return False

    with open(src_file, 'rb') as f:
        lzma_data = f.read()

    if len(lzma_data) < 16:
        print(f"  ERROR: LZMA data too short")
        return False

    head_len = 13
    align_len = 16

    if lzma_data[0:head_len] == b'\x00' * head_len:
        print(f"  WARNING: LZMA header appears to be empty/padded")
        return False

    unaligned_data = bytearray(len(lzma_data) - (align_len - head_len))
    unaligned_data[0:head_len] = lzma_data[0:head_len]
    unaligned_data[head_len:] = lzma_data[align_len:]

    temp_file = src_file + ".tmp"
    with open(temp_file, 'wb') as f:
        f.write(unaligned_data)

    cmd = [tool_path, "d", temp_file, dst_file]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
        os.remove(temp_file)
        if result.returncode == 0:
            print(f"  Decompressed: {src_file} -> {dst_file}")
            return True
        else:
            print(f"  ERROR: LZMA decompression failed: {result.stderr}")
            return False
    except Exception as e:
        if os.path.exists(temp_file):
            os.remove(temp_file)
        print(f"  ERROR: Failed to run LZMA tool: {e}")
        return False

def parse_upg_package(input_file, output_dir="extracted"):
    os.makedirs(output_dir, exist_ok=True)

    with open(input_file, 'rb') as f:
        data = bytearray(f.read())

    print(f"Parsing upgrade package: {input_file}")
    print(f"Output directory: {output_dir}")
    print(f"Package size: 0x{len(data):X} ({len(data)} bytes)")

    fota_key_area_data._fields_ = fota_key_area_data_fields
    fota_info_area_data._fields_ = fota_info_area_data_fields

    fota_key_head = fota_key_area_data.from_buffer(data)
    key_area_len = int(fota_key_head.struct_length)

    print(f"\nFOTA Key Area:")
    print(f"  image_id: 0x{fota_key_head.image_id:08X}")
    print(f"  struct_version: 0x{fota_key_head.struct_version:08X}")
    print(f"  struct_length: 0x{fota_key_head.struct_length:X} ({key_area_len} bytes)")
    print(f"  key_alg: 0x{fota_key_head.key_alg:08X}")
    print(f"  fota_key_version_ext: 0x{fota_key_head.fota_key_version_ext:08X}")
    print(f"  mask_fota_key_version_ext: 0x{fota_key_head.mask_fota_key_version_ext:08X}")
    print(f"  fota_info_addr: 0x{fota_key_head.fota_info_addr:X}")

    fota_info_offset = key_area_len
    if fota_key_head.fota_info_addr != 0:
        fota_info_offset = int(fota_key_head.fota_info_addr)

    fota_info_head = fota_info_area_data.from_buffer(data[fota_info_offset:fota_info_offset + 1024])
    info_area_len = int(fota_info_head.struct_length)

    print(f"\nFOTA Info Area:")
    print(f"  image_id: 0x{fota_info_head.image_id:08X}")
    print(f"  struct_version: 0x{fota_info_head.struct_version:08X}")
    print(f"  struct_length: 0x{fota_info_head.struct_length:X} ({info_area_len} bytes)")
    print(f"  fota_version_ext: 0x{fota_info_head.fota_version_ext:08X}")
    print(f"  image_hash_table_addr: 0x{fota_info_head.image_hash_table_addr:X}")
    print(f"  image_hash_table_length: 0x{fota_info_head.image_hash_table_length:X}")
    print(f"  image_num: {fota_info_head.image_num}")
    print(f"  hardware_id: 0x{fota_info_head.hardware_id:08X}")

    image_num = fota_info_head.image_num
    hash_table_offset = fota_info_offset + info_area_len
    if fota_info_head.image_hash_table_addr != 0:
        hash_table_offset = int(fota_info_head.image_hash_table_addr)

    print(f"\nHash Table (offset: 0x{hash_table_offset:X}):")

    hash_node_size = sizeof(fota_image_hash_node)
    extracted_images = []

    for i in range(image_num):
        hash_node = fota_image_hash_node.from_buffer(data[hash_table_offset + i * hash_node_size:])
        print(f"\nImage {i}:")
        print(f"  image_id: 0x{hash_node.image_id:08X}")
        print(f"  image_addr: 0x{hash_node.image_addr:X}")
        print(f"  image_length: 0x{hash_node.image_length:X} ({hash_node.image_length} bytes)")

        image_head = fota_image_head.from_buffer(data[int(hash_node.image_addr):])
        image_offset = int(image_head.image_offset)
        image_len = int(image_head.image_len)

        print(f"  header_magic: 0x{image_head.header_magic:08X}")
        print(f"  image_offset: 0x{image_offset:X}")
        print(f"  image_len: 0x{image_len:X}")
        print(f"  new_image_len: 0x{image_head.new_image_len:X}")
        print(f"  decompress_flag: 0x{image_head.decompress_flag:08X}")

        image_data_start = image_offset
        image_data_end = image_offset + image_len

        if image_data_end > len(data):
            print(f"  WARNING: Image data exceeds package size, adjusting...")
            image_data_end = len(data)

        image_data = data[image_data_start:image_data_end]

        image_id_hex = f"{hash_node.image_id:08X}"
        is_lzma = (image_head.decompress_flag == LZMA_COMPRESS_FLAG)

        if is_lzma:
            output_file = os.path.join(output_dir, f"{image_id_hex}_lzma.bin")
        else:
            output_file = os.path.join(output_dir, f"{image_id_hex}.bin")

        with open(output_file, 'wb') as f:
            f.write(image_data)
        print(f"  Extracted to: {output_file}")

        if is_lzma:
            decompressed_file = os.path.join(output_dir, f"{image_id_hex}.bin")
            print(f"  Attempting to decompress...")
            if decompress_lzma(output_file, decompressed_file):
                output_file = decompressed_file
                print(f"  Keeping compressed file: {os.path.join(output_dir, image_id_hex + '_lzma.bin')}")
            else:
                print(f"  WARNING: Decompression failed, keeping compressed file")

        extracted_images.append({
            'index': i,
            'image_id': hash_node.image_id,
            'image_id_hex': image_id_hex,
            'output_file': output_file,
            'size': len(image_data),
            'new_image_len': image_head.new_image_len,
            'decompress_flag': image_head.decompress_flag,
            'is_lzma': is_lzma,
            'offset_start': image_data_start,
            'offset_end': image_data_end,
        })

    manifest_file = os.path.join(output_dir, "manifest.txt")
    with open(manifest_file, 'w') as f:
        f.write(f"Upgrade Package Manifest\n")
        f.write(f"========================\n\n")
        f.write(f"Input file: {input_file}\n")
        f.write(f"Total images: {image_num}\n\n")

        f.write(f"FOTA Key Area (up to and including fota_info_addr):\n")
        f.write("-" * 50 + "\n")
        f.write(f"  image_id: 0x{fota_key_head.image_id:08X}\n")
        f.write(f"  struct_version: 0x{fota_key_head.struct_version:08X}\n")
        f.write(f"  struct_length: 0x{fota_key_head.struct_length:X} ({key_area_len} bytes)\n")
        f.write(f"  signature_length: 0x{fota_key_head.signature_length:08X}\n")
        f.write(f"  key_owner_id: 0x{fota_key_head.key_owner_id:08X}\n")
        f.write(f"  key_id: 0x{fota_key_head.key_id:08X}\n")
        f.write(f"  key_alg: 0x{fota_key_head.key_alg:08X}\n")
        f.write(f"  ecc_curve_type: 0x{fota_key_head.ecc_curve_type:08X}\n")
        f.write(f"  key_length: 0x{fota_key_head.key_length:08X}\n")
        f.write(f"  fota_key_version_ext: 0x{fota_key_head.fota_key_version_ext:08X}\n")
        f.write(f"  mask_fota_key_version_ext: 0x{fota_key_head.mask_fota_key_version_ext:08X}\n")
        f.write(f"  msid_ext: 0x{fota_key_head.msid_ext:08X}\n")
        f.write(f"  mask_msid_ext: 0x{fota_key_head.mask_msid_ext:08X}\n")
        f.write(f"  maintenance_mode: 0x{fota_key_head.maintenance_mode:08X}\n")
        f.write(f"  die_id: {bytes(fota_key_head.die_id).hex()}\n")
        f.write(f"  fota_info_addr: 0x{int(fota_key_head.fota_info_addr):X}\n\n")

        f.write(f"FOTA Info Area (up to and including hardware_id):\n")
        f.write("-" * 50 + "\n")
        f.write(f"  image_id: 0x{fota_info_head.image_id:08X}\n")
        f.write(f"  struct_version: 0x{fota_info_head.struct_version:08X}\n")
        f.write(f"  struct_length: 0x{fota_info_head.struct_length:X} ({info_area_len} bytes)\n")
        f.write(f"  signature_length: 0x{fota_info_head.signature_length:08X}\n")
        f.write(f"  fota_version_ext: 0x{fota_info_head.fota_version_ext:08X}\n")
        f.write(f"  mask_fota_version_ext: 0x{fota_info_head.mask_fota_version_ext:08X}\n")
        f.write(f"  msid_ext: 0x{fota_info_head.msid_ext:08X}\n")
        f.write(f"  mask_msid_ext: 0x{fota_info_head.mask_msid_ext:08X}\n")
        f.write(f"  image_hash_table_addr: 0x{int(fota_info_head.image_hash_table_addr):X}\n")
        f.write(f"  image_hash_table_length: 0x{int(fota_info_head.image_hash_table_length):X}\n")
        f.write(f"  image_hash_table_hash: {bytes(fota_info_head.image_hash_table_hash).hex()}\n")
        f.write(f"  image_num: {fota_info_head.image_num}\n")
        f.write(f"  hardware_id: 0x{fota_info_head.hardware_id:08X}\n\n")

        f.write(f"Extracted Images:\n")
        f.write("-" * 50 + "\n")
        for img in extracted_images:
            f.write(f"\nImage {img['index']}:\n")
            f.write(f"  Image ID: {img['image_id_hex']}\n")
            f.write(f"  Offset in package: 0x{img['offset_start']:X} - 0x{img['offset_end']:X} (0x{img['offset_end'] - img['offset_start']:X} bytes)\n")
            f.write(f"  Compressed file: {os.path.join(output_dir, img['image_id_hex'] + '_lzma.bin')}\n")
            f.write(f"  Decompressed file: {img['output_file']}\n")
            f.write(f"  Compressed size: 0x{img['size']:X} ({img['size']} bytes)\n")
            f.write(f"  Original size: 0x{img['new_image_len']:X}\n")
            f.write(f"  Decompress flag: 0x{img['decompress_flag']:08X}\n")
            f.write(f"  Is LZMA compressed: {img['is_lzma']}\n")

    print(f"\n--- Summary ---")
    print(f"Total images extracted: {len(extracted_images)}")
    print(f"Manifest saved to: {manifest_file}")

    return True


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 parse_upg_package.py <input.upg> [output_dir]")
        print("")
        print("Parse an upgrade package and extract images by image_id")
        print("")
        print("Arguments:")
        print("  input.upg      - Input upgrade package file")
        print("  output_dir     - Output directory (default: ./extracted)")
        return 1

    input_file = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) > 2 else "extracted"

    if not os.path.exists(input_file):
        print(f"Error: File not found: {input_file}")
        return 1

    success = parse_upg_package(input_file, output_dir)
    return 0 if success else 1


if __name__ == "__main__":
    sys.exit(main())