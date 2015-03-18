#Buildbump
Utility for bumbing build version before compiling an application or library

##Features
###File
With the `-f <file>` or `--file <file>` option, you can specify which file to perform bumping on. If this option is left out, the utility will look for `version.h` in the current working directory.
###Regular expressions
With the `-p <pattern>` or `--pattern <pattern>` option, you can specify a regular expression to match when looking for the build version.
The default expression is `(.*VER_BUILD\s+)(\d+)` ([Example matches](http://www.regexr.com/3akkd))
**Notice** how there are **two** captures in the expression. The first one is the entire line before the actual build number, and the second is for the build number itself. This criteria must always be met, or else the utility won't be able to replace text correctly.
###Silence!
When the `-s` or `--silent` option is set, the utility generates no output at all.

##Return codes
| Return code  | Second Header                    |
| ------------ | -------------------------------- |
| 0            | Normal exit                      |
| 1            | Failed to open file for reading  |
| 2            | Failed to open file for writing  |
| 3            | No rows affected                 |

##Features to be added

 - Recursive search for the specified file
 - More verbose output
 - Option for specifying how much to increment version number
 - Clean up output-related code
