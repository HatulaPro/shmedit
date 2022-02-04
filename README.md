# shmedit
A text editor program for windows.

## Warning:
  I have no idea what I'm doing, and I am not responsible for what might happen as a result of you using it.
  Use this at your own risk.

## Why Make It?
  I like vim. I like Visual Studio Code. I'm just taking features that I like from both worlds.
  You might even find it useful, so have at it. 
  

## Documentation:
  - Currently, there is no way to customize keybinds. This feature will be added in the future though. 
  
  - Notice that Shmedit is still in development, so many useful features (such as copy & paste) do not exist yet.
  
  - `mem`: The editor's memory. 
  
  - Keys:
    - You can write by simply typing letters and numbers. Pretty normal.
    - `Delete`, `Remove` keys - Just like you'd expect.
    - `Ctrl + Delete`: Remove the word to the right of your cursor and saves its contents in `mem`. 
    - `Ctrl + Remove`: Remove the word to the left of your cursor.
    - `Left`, `Right`, `Up`, `Down` keys: Standard navigation. 
    - `Ctrl + Left`, `Ctrl + Right`: Move one whole word to that direction.
    - `Alt + Up`, `Alt + Down`: Move current line up/down.
    - `Ctrl + S`: Save file.
    - `Ctrl + Enter`: Leave current line and start a new one.
    - `Ctrl + C`: Start command mode
    - `Ctrl + D`: Duplicate current line
  
  - Commands:
    After starting command mode (`Ctrl + C`), you can start writing commands. The editor will parse and run them.
    Press `Enter` to start running the command, or `Ctrl + C` to cancel it.
    
    - `s`: Save file.
      
      `cmd| s`
    - `o <file name>`: Opens the requested file.
      
      `cmd| o myfile.txt`
    - `q`: Quits the editor. Warning: This command does not save the contents of the file.
    - `qs`: Saves and quits the editor. 
    - `dw`: Deletes the next word and saves its contents in `mem`. 
    - `dl`: Deletes the current line and saves its contents in `mem`. 
    - `p`: Pastes the last word from `mem`.
      
## Future Features:
  [X] A working text editor
  [ ] Find & Replace
  [ ] Copy & Paste
  [ ] Custom keybindings
  [ ] Highlighting
  [ ] Custom commands
  [ ] Making each user a millionaire
  
