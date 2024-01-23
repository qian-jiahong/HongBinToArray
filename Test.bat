@echo Off

HongBinToArray.exe "ChineseChess.nes" "ChineseChess.h"
if %ERRORLEVEL% LSS 0 (
    set ERROR_MSG=´íÎó %ERRORLEVEL%
    goto error
)
goto done

:error
echo %ERROR_MSG%
REM timeout /t 1 /nobreak >nul
REM exit 10

:done
REM exit 0