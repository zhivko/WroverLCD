set project_folder=c:\msys32\home\klemen\esp\blink

echo %project_folder%

python c:\msys32\home\klemen\esp\esp-idf-v3.1.1\components\esptool_py\esptool\esptool.py --chip esp32 --port COM28 --baud 921600 --before "default_reset" --after "hard_reset" write_flash --compress -z --flash_mode "dio" --flash_freq "40m" --flash_size detect 0x10000 "%project_folder%\build\blink.bin"
