#include <fcntl.h>
#include <jni.h>
#include <string.h>
#include <sys/ioctl.h>

#define FAIL -1
#define SUCCESS 1

#define UP_TEXT "Up"
#define DOWN_TEXT "Down"
#define LEFT_TEXT "Left"
#define RIGHT_TEXT "Right"
#define CENTER_TEXT "Center"

#define LED_PATH "/dev/sm9s5422_led"
#define VIBRATOR_PATH "/dev/sm9s5422_perivib"
#define PIEZO_PATH "/dev/sm9s5422_piezo"
#define SEGMENT_PATH "/dev/sm9s5422_segment"
#define PUSHBUTTON_PATH "/dev/sm9s5422_interrupt"
#define TEXTLCD_PATH "/dev/sm9s5422_textlcd"

#define TEXTLCD_BASE 0x56
#define TEXTLCD_FUNCTION_SET _IO(TEXTLCD_BASE, 0x31)

#define TEXTLCD_DISPLAY_ON _IO(TEXTLCD_BASE, 0x32)
#define TEXTLCD_DISPLAY_OFF _IO(TEXTLCD_BASE, 0x33)
#define TEXTLCD_DISPLAY_CURSOR_ON _IO(TEXTLCD_BASE, 0x34)
#define TEXTLCD_DISPLAY_CURSOR_OFF _IO(TEXTLCD_BASE, 0x35)

#define TEXTLCD_CURSOR_SHIFT_RIGHT _IO(TEXTLCD_BASE, 0x36)
#define TEXTLCD_CURSOR_SHIFT_LEFT _IO(TEXTLCD_BASE, 0x37)

#define TEXTLCD_ENTRY_MODE_SET _IO(TEXTLCD_BASE, 0x38)
#define TEXTLCD_RETURN_HOME _IO(TEXTLCD_BASE, 0x39)
#define TEXTLCD_CLEAR _IO(TEXTLCD_BASE, 0x3a)

#define TEXTLCD_DD_ADDRESS_1 _IO(TEXTLCD_BASE, 0x3b)
#define TEXTLCD_DD_ADDRESS_2 _IO(TEXTLCD_BASE, 0x3c)
#define TEXTLCD_WRITE_BYTE _IO(TEXTLCD_BASE, 0x3d)

int led_fd, vibrator_fd, piezo_fd, segment_fd, textlcd_fd;
