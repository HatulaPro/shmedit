# Shmedit
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
  
  - ### Keys:
    - You can write by simply typing letters and numbers. Pretty normal.
    - `Delete`, `Backspace` keys - Just like you'd expect.
    - `Ctrl + Delete`: Remove the word to the right of your cursor and saves its contents in `mem`. 
    - `Ctrl + Remove`: Remove the word to the left of your cursor.
    - `Left`, `Right`, `Up`, `Down` keys: Standard navigation. 
    - `Ctrl + Left`, `Ctrl + Right`: Move one whole word to that direction.
    - `Alt + Up`, `Alt + Down`: Move current line up/down.
    - `Ctrl + S`: Save file.
    - `Ctrl + Enter`: Leave current line and start a new one.
    - `Ctrl + C`: Start command mode.
    - `Ctrl + D`: Duplicate current line.
    - `Ctrl + L`: Delete current line.
    - `FN + Right`, `FN + LEFT`: Jump to the end/start of the line.
    - `Ctrl + ]`: Tabify line.
    - `Ctrl + [`: Untabify line.
    - `Ctrl + F`: The command 'f: ' will be set, to allow users to get to `find` mode more easily.
    - `Ctrl + R`: The command 'fr: ' will be set, to allow users to get to `find&rep` mode more easily.
    - `Ctrl + V`: Switches to `visual` mode.
      + When in `visual` mode, you'll have two cursors. A static one (set to the location where you pressed `Ctrl + V`), and a dynamic one, which you can control.
      + Controlling the `visual` mode dynamic cursor is done in the same way as normally, altough other modes are not available.
      + Visual mode functions:
        - `c`: Copy selection.
        - `r`, `Backspace`: Remove selection.
        - `p`: Paste from `mem`. Will be pasted after the dynamic cursor.
        - `Alt + Up`: Move selection one line up.
        - `Alt + Down`: Move selection one line down.
        - `Ctrl + L`: Select select lines from start to end.
  
  
  - ### Commands:
    After starting command mode (`Ctrl + C`), you can start writing commands. The editor will parse and run them.
    Press `Ctrl + C` to cancel your command, or `Enter` to exit `cmd` mode.
    There are two kinds of commands. `Called commands` and `Instant commands`.
      - `Called commands`: Always start with a colon (`:`), and are only called once `Enter` is pressed. Called commands may take parameters as well.
      - `Instant commands`: Are called as soon as they are detected. Can be prefixed by a number to tell the parser the amount of times it should be run.
        + Example:
        ```
          cmd| 12dw
        ```
        Would delete 12 words. Keep in mind that only the last word deleted will be saved in `mem`.
    
    
    #### Called commands:    
    - `o <file name>`: Opens the requested file.
    
      ```
      cmd| :o myfile.txt
      ```
    
    - `f <str>`: Starts `find` mode. Press `Enter` or `n` to find the next instance of `<str>`, or any other key to go back to deafult mode.
    
       ```
       cmd| :f StrToLookFor
       ```
    
    - `fr <str>~<str>`: Starts `find&rep` mode. Press `Enter` or `n` to find the next instance of the first argument (the one before the `~`), and press them again to replace. Pressing `s` will skip the current instance. Press any other key to go back to deafult mode.
      
      ```
      cmd| :fr StrToLookFor~StrToBeReplacedWith
      ```
    
    - `<Integer>`: Type any integer and then `Enter` to jump to that line. For example, the following command will move the cursor to line 45:
    
      ```
      cmd| 45
      ```      
    
    #### Instant commands:    
    - `s`: Save file.
    - `i`, `Enter`: Exit cmd mode.
    - `q`: Saves and quits the editor. 
    - `Q`: Quits the editor. Warning: This command does not save the contents of the file.
    - `dw`: Deletes the next word and saves its contents in `mem`. 
    - `dl`: Deletes the current line and saves its contents in `mem`. 
    - `db`: Removes the last word and saves its contents in `mem`. 
    - `p`: Pastes the last word from `mem`.
    - `w`: Moves one word to the left.
    - `b`: Moves one word to the right.
    - `ta`: Tabify line.
    - `ua`: Unabify line.
    - `j`: Move the cursor one line down.
    - `k`: Move the cursor one line up.
    - `h`: Move the cursor one character to the left.
    - `l`: Move the cursor one character to the right.
    - `cw`: Copy the next word to `mem`.
    - `cl`: Copy the next line to `mem`.
    - `cw`: Copy the last word to `mem`.
    
## Future Features:
  - [X] A working text editor
  - [X] Find & Replace
  - [X] Visual mode 
  - [X] Copy & Paste
  - [ ] Custom keybindings
  - [ ] Highlighting
  - [ ] Custom commands
  - [ ] Making each user a millionaire
