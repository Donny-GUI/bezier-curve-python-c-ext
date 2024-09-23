REM Define variables
SET EXTENSION_NAME=bezier
SET SOURCE_FILE=bezier.c
SET SETUP_FILE=setup.py

REM Check if Python is installed
python --version >nul 2>&1
IF ERRORLEVEL 1 (
    echo Python is not installed. Please install Python and ensure it's added to your PATH.
    exit /b 1
)

REM Check if setuptools is installed
echo checking for setuptools
python -c "import setuptools" >nul 2>&1
IF ERRORLEVEL 1 (
    echo setuptools is not installed. Installing setuptools...
    pip install setuptools
)

REM Build the bezier C extension
echo Building bezier C extension...
python setup.py build
IF ERRORLEVEL 1 (
    echo Failed to build the C extension.
    exit /b 1
)

REM Install the bezier extension
echo Installing the C extension...
python setup.py install
IF ERRORLEVEL 1 (
    echo Failed to install the C extension.
    exit /b 1
)

echo C extension installed successfully.
echo import with 'import bezier' or 'from bezier import *'

ENDLOCAL
pause
