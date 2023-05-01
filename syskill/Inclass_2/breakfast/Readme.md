Task I
1. In your home directory somewhere on your home directory (either on macOS or Linux), create a directory named breakfast.
cd <to destination file> mkdir breakfast 
2. Change to the breakfast folder.
cd breakfast
3. Create two empty files, file1.txt and file2.txt.
	vim file1.txt / vim file2.txt OR cat > file1.txt (Done Ctrl + D)
4. Randomly copy three more files into this folder, text file or not. (If you need text files, copy them from http://www.textfiles.com/directory.htmlLinks to an external site.)
5. Create a directory named messy.
	mkdir messy
6. Create an empty file file3.txt.
	vim file3.txt cat > file3.txt


Task II
1. Check if a file exists. Print a message stating if the file exists or not.
#!/bin/bash
filename="file3.txt"
if [ -e "$filename" ]; then
    echo "$filename exists as a file"
fi
2. Read from a given text file and print the content on the screen with one blank line after each line in the file (like double-spacing the content). Make sure your script checks if the file exists before the actual operation. If not, report the error. Test this script with messy/file3.txt
	cat file3.txt
3. List the path of all files (no directory) in the working directory non-recursively using a for loop (without using ls or find).
#!/bin/bash
filedir=/Users/aru/Desktop/Projects/breakfast 
for path in "$filedir"/*
do
  echo "$path"
done
4. List the size and path of all files in the working directory non-recursively using find. (macOS)
find . -exec du -a {} +
5. List the path of files with the extension "txt" in the home directory.
find ~ -type f -name "*.txt" (~ for home/. for current dir)
