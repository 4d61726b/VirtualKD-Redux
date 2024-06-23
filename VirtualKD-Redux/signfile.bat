IF EXIST %1 (
    IF EXIST %2 (
        %WDK7_PATH%\bin\amd64\signtool.exe sign /v /f %1 /t http://timestamp.digicert.com /fd sha256 %2
    ) ELSE (
        echo ERROR: Invalid arguments, %2 is not a valid file
        exit 1
    )
) ELSE (
    echo ERROR: Missing %1. Use gencerts.bat to generate certificates
    exit 1
)