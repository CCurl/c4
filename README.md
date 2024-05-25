# A register-based interpreter

There are 26 registers, 'a' .. 'z'
Register 'rA' is the "accumulator" (ACC).
Numbers go into this register. 

##Examples: 
```
"1234 .A" prints "1234"
```

## Reference
```
.A			Print ACC (eg - "65 .A" prints "65")
.[A-Z]		Print r<X> (eg - "123:B .B" prints "123")
,[A-Z]		Print r<X> as a character (eg - "65,A" prints "A")

[0-9]*		Parse the number into ACC
[A-Z]       Copy the value of r<X> into ACC
:[A-Z]		Copy the value of ACC into r<R>

+[A-Z]		ACC = ACC + r<X> (eg = "2:C 45+C .A" prints "47")
+[0-9]*		ACC = ACC + NNNN (eg = "45:C C+2 .A" prints "47")

-[A-Z]		ACC = ACC - r<X> (eg = "3:B 25-B .A" prints "22")
-[0-9]*		ACC = ACC - NNNN (eg = "25:B B-3 .A" prints "22")

*[A-Z]		ACC = ACC * r<X> (eg = "3:B 5*B  .A" prints "15")
*[0-9]*		ACC = ACC * NNNN (eg = "5:B B*3  .A" prints "15")

/[A-Z]		ACC = ACC / r<X> (eg = "3:C 33/C .A" prints "11")
/[0-9]*		ACC = ACC / NNNN (eg = "33:C C/3 .A" prints "11")

![A-Z]      Store ACC to address r<X> (eg - "22:B 3456:B" stores 3456 to address 22)
![0-9]      Store ACC to address NNNN (eg - "12345 !15" stores 12345 to address 15)
@[A-Z]      Fetch value from address r<X> into ACC (eg = "33:B @B" fetches the value from address 33)
@[0-9]      Fetch value from address NNNN into ACC (eg = "@33" fetches the value from address 33)

~			Exit
```
