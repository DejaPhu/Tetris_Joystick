#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -std=c99 -Wall -Wextra -Werror -Wno-misleading-indentation -fsanitize=address,undefined -g -Itests/host_stubs -Iapp/inc tests/test_game.c app/src/game_engine.c app/src/game_fsm.c app/src/button_filter.c app/src/ui_renderer.c -o build/host/test_game
build/host/test_game
cc -std=c99 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Itests/uart_stubs -Ihal/inc -Iapp/inc tests/test_uart.c hal/src/hal_uart.c app/src/app_log.c -o build/host/test_uart
build/host/test_uart
cc -std=c99 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Iapp/inc tests/test_joystick.c app/src/joystick_filter.c app/src/button_filter.c -o build/host/test_joystick
build/host/test_joystick
cc -std=c99 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Itests/joystick_stubs -Ihal/inc tests/test_joystick_adc.c hal/src/hal_joystick.c -o build/host/test_joystick_adc
build/host/test_joystick_adc
cc -std=c99 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Itests/joystick_stubs -Ihal/inc -Iapp/inc tests/test_joystick_task.c app/src/task_input.c app/src/joystick_filter.c app/src/button_filter.c -o build/host/test_joystick_task
build/host/test_joystick_task
