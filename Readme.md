# comparison program

Collaborative development of different programs that solve a more complex and 
expressive problem. The programs are implemented in different languages with 
different development systems and paradigms. 

The goal is not to find the "best" language, but to have a comparability, to 
work out advantages and disadvantages of languages and paradigms and to show a 
qualification need of the user groups in the respective languages.

# the idea

As comparisons between programming languages and paradigms often fail due to
the lack of comparable programs, only individual (micro-) algorithms or general
considerations can be made. A few days ago, I came up with the idea for this
program that can be implemented with various programming languages within a
manageable time frame. My goal is to enable a comparability of programming
languages, development systems within a language, algorithm quality, and 
different programming paradigms. Therefore, I want to initiate a distributed 
project in which developers with different programming languages, skill levels,
and development tools can participate. The primary goal is not to compare
programming languages; we can work together to identify advantages and
disadvantages, evaluate different algorithms and paradigms, and make visible
the need for qualifications in various user groups.

When considering this program, it was important that the task should be 
manageable within a reasonable time frame, as otherwise, there may be little 
participation. Nevertheless, the application must be complex enough to provide 
real comparability, various requirements must be combined, and the program should 
work with a larger data set. As with any example, there can be a discussion about 
the usefulness of the program. I also tried to avoid technologies that depend more 
on central systems and drivers written in other languages (mostly in assembler or C/C++).

Therefore, this project is about the ability to sequentially read data from files in 
different programming languages, provide it in dynamic memory structures for processing, 
sort it, perform more complex calculations, evaluate data, and write it back to sequential 
files. In a second area, a comprehensive directory structure should also be created and 
read again to demonstrate the differences in working with directory structures with many 
different files and file operations.

Each participant should implement the project with their current knowledge. However, this 
type of project is also very suitable for learning and deepening one's own skills. 
Later on, you can simply try out new techniques in a second attempt. 
Both programs should then be submitted.

Only the means of the respective programming languages should be used for the languages 
themselves. For example, it would be incorrect to use a specific native framework in 
JavaScript or a special library written in C in Python, with the justification that the 
differences to C/C++ are not so bad. If there is no other option with a language, a note 
should be made about its use.

# Task description

The idea of the program is to work with a larger number of addresses (cadastre data) with 
geographic coordinates and geodetic calculations. The system initially consists of only two 
independent data files. All input and output files are stored in a central working directory.
The file with Berlin cadastre data (all concrete addresses with geo-coordinates in Berlin, 
as of approximately 2010), which contains 384,860 records, and another file with Berlin 
hotspots (called "Places of Interest" in English), such as monuments, museums, theaters, 
bodies of water, hospitals, universities, etc. This file is currently being continuously 
updated by me and contains approximately 260 records. I create this file manually, but since 
I don't know all the hotspots in Berlin, it's difficult and unfortunately time-consuming. Hence, 
the continuous updates.

Both files are created in the national character set (western european). While the data in the 
first file is stored separated by semicolons, and thus has a variable line length, I intentionally 
use a fixed sentence length for the second file.

Both files are to be used as is and must not be modified before use. In reality, we regularly 
receive data deliveries in different formats that we have to process regularly.

I deliberately chose not to use an XML or JSON file because in this case, foreign libraries will 
definitely be used, which were most likely created in a different language or technology. 
The same applies to the use of databases, where the respective drivers play a bigger role than 
the programming languages and technologies used in these languages.

The files are located in a working directory independent of the program and must not be changed 
during processing. All outputs are also only in this directory. At each work step, the time in 
milliseconds[^time_mesuarement] with 3 decimal places will be output (microseconds / 1000), and 
at the end, the time from the start of the program to completion can be output centrally.

During processing, the data is stored in a dynamic array (storage in a contiguous memory area that 
increases/decreases in size), sorted in it, and all calculations are performed.

# C++

* Berlin_App - Implementation with Visual C++
* from Stream Evolution of C++ (Mostly Independend)
  * ChatGPT - created with the help of ChatGPT
  * Modern C++ - iterative created in the stream with modern C++ 

# Delphi

* Programm_AnsiString \
initial implementation of the task with national chars and Ansistring  
* Programm_String \
adjustment after protest from Embarcadero that the task would not be timely

Copyright 2021 - 2023 adecc Systemhaus GmbH
Author Volker Hillmann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.


[^time_mesuarement]: If no exact time specifications are possible in the respective 
programming language, rounding up is always necessary (for example, if only 
milliseconds can be specified, 1 millisecond must be added to the time difference
