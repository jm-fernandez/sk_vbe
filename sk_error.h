#ifndef SK_ERROR_H_
#define SK_ERROR_H_

typedef enum sk_error_t_
{
  sk_success                            = 0,
  sk_error_command_line                 = 1,
  sk_error_vbe_installed                = 2,
  sk_error_vbe_not_installed            = 3,
  sk_error_vbe_driver_not_found         = 4,
  sk_error_vbe_mode_not_found           = 5,
  sk_error_vbe_controller_info_error    = 6,
  sk_error_too_many_video_modes         = 7,
  sk_error_video_mode_already_in_use    = 8,
  sk_error_multiplex_interrupt          = 9,
  sk_error_unknown                      = 10,
} sk_error_t;

#define SK_SUCCESS(error) (error == sk_success)

const char* sk_error_description(sk_error_t error);

#endif // SK_ERROR_H_
