INCLUDE board.ld

SECTIONS
{
    g_usb_ram_begin = ORIGIN(USB_DTCM);
    g_usb_ram_size = LENGTH(USB_DTCM);
}