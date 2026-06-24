# Customized DCC-EX firmware

## Software
- Fork the DCC-EX repository from https://github.com/DCC-EX/CommandStation-EX.git
- Clone the repository to your local machine.
- To make it easier, add an upstream remote pointing at the original repository: ```git remote add upstream git@github.com:DCC-EX/CommandStation-EX.git```
- From the upstream repository, pull the latest changes and tags: `git fetch upstream --tags`
- Checkout the tag for the release build.
- Open the project in VS Code.
- If needed, install PlatformIO IDE extension for VS Code.
- Click on the PlatformIO icon in the left sidebar.
- Select the STM32 Nucleo-F439ZI board from the list of available boards.
- Update the config.h and myAutomation.h file as appropriate for the selected board.
    - See `git@github.com:ckier/dcc-ex-configs.git`
- Build the project.
- Upload the firmware to the STM32 Nucleo-F439ZI board.
    - For direct USB connection, select the "Upload" task in the PlatformIO sidebar.
    - For remote upload, select the "Remote Upload" task in the PlatformIO sidebar.

## STM32 Nucleo-F439ZI
- USB Upload
    - The PlatformIO framework needs external power to load firmware onto the Nucleo board using the standard USB connection.
    - Use the DCC-EX Motorshield to provide the required power by using it to feed a 12V supply to the main board.
    - Plug the Nucleo board to the computer using a USB cable.
    - For Linux OS (including arm 64), no additional drivers are needed for the Nucleo board. The board will be recognized automatically by PlatformIO during upload.
    - For Windows OS, install the STM32 drivers from https://www.st.com/en/development-tools/stsw-stm32102.html.
- PIO CLI Remote Upload
    - On remote machine with access to the Nucleo board via USB cable.
    - Create virtual python environment to install PlatformIO.
    - Install PlatformIO: `pip install platformio`
    - Activate the python environment: `source venv/bin/activate`
    - Login to PlatformIO Account: `pio account login`
    - Start the agent: `pio agent start`
    - Use bash job control to manage the agent.
        - Press `Ctrl+Z` to suspend the agent
        - Use `bg` to resume it in the background
        - Use `fg` to bring it back to the foreground
        - Use `jobs` to list all background jobs
        - Use `kill %1` (replace 1 with the job number) to terminate the agent
        - Use `Ctrl+C` to kill the agent
    - List remote devices: `pio device list`
