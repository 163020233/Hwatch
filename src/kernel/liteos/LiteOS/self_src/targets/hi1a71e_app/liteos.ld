INCLUDE board.ld

SECTIONS
{
    g_usb_ram_begin = ORIGIN(USB_L2M);
    g_usb_ram_size = LENGTH(USB_L2M);
}