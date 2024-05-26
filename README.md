# c4: a portable Forth system inspired by Tachyon

In c4, a program is a sequence of word-codes. <br/>
A word-code is an unsigned 16-bit unsigned value (0..65535). <br/>
Stack entries are either 32-bit or 64-bit values. <br/>
If a word-code is less than or equal to `BYE`, it is a code primitive. <br/>
If it is greater than `BYE`, it is the address of a word to execute. <br/>

## C4 memory areas

The code area can store up to 65536 word-codes. (CODE-SZ) <br/>
The dictionary area can be up to 553
