## Overview
- The program will read in lines of characters from the standard input and write them as 80 character long lines to standard output with the following changes:
1. Every line separator in the input will be replaced by a space
2. Every adjacent pair of plus signs, i.e., "++", is replaced by a "^".
### Input
- A “line of input” is
  - A sequence of the allowed characters (see below) that does not include a line separator
- Followed by a line separator.
- Allowed characters: Other than the line separator, the input will only consist of ASCII characters from space (decimal 32) to tilde (decimal 126). These are sometimes termed printable characters.
- The input will not contain any empty lines, i.e., lines that only have space characters or no characters except the line separator.
- An input line will never be longer than 1000 characters (including the line separator).
- Your program should process input lines until it receives an input line that contains only the characters DONE (followed immediately by the line separator) at which point the program should terminate.
  - Examples: The following input lines should not terminate the program
  - DONE!
  - ALLDONE
### OutPut
 - The “80 character line” to be written to standard output is defined as 80 non-line separator characters plus a line separator.
 - Whenever your program has sufficient data for an output line, the output line must be produced. Your program must not wait to produce the output only when the terminating line is received.
 - No part of the terminating line should be written to the output
 - Your program should output only lines with 80 characters (with a line separator after each line).
  - For example,
    - If your program has some input that has not been written because the program is waiting for more data in order to make an 80 character line for output and the terminating line is received from standard input,
    - Then the program should terminate without printing out the partial output line.
  - In addition, your program should not output any user prompts, debugging information, status messages, etc.
  - For the second replacement requirement, pairs of plus signs should be replaced as they are seen.
    - Examples:
      - The string “abc+++def” contains only one pair of plus signs and should be converted to the string "abc^+def".
      - The string “abc++++def” contains two pairs of plus signs and should be converted to the string "abc^^def".
### Multi-threading Requirement
Your program will create 4 threads as follows:

1. Input Thread: This thread performs input on a line-by-line basis from standard input.
2. Line Separator Thread: This thread replaces line separators with blanks.
3. Plus Sign Thread: This thread performs the required replacement of pair of plus signs.
4. Output Thread: This thread writes output lines to the standard output.

recieved 125/90 on this
