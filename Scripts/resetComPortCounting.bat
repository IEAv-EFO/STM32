@echo off
setlocal enabledelayedexpansion

set "reg_file=reset_com_ports.reg"
set "reg_content=Windows Registry Editor Version 5.00\n\n"
set "found_keys="

for /f "tokens=*" %%a in ('reg query "HKLM\SYSTEM\CurrentControlSet\Services\Serial\Enum" /s /f "0000" /c 2^>nul ^| find /i "REG_SZ"') do (
    set "subkey=%%a"
    set "subkey=!subkey:    =!"
    set "subkey=!subkey:\=\\!"
    set "found_keys=!found_keys!,!subkey!"
    set "reg_content=!reg_content![-HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Serial\Enum\!subkey!]\n"
)

if "%found_keys%"=="" (
    echo No COM port keys found.
    exit /b
)

echo !reg_content! > "%reg_file%"
echo Registry script created: %reg_file%
