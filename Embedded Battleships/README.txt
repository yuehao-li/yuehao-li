/*********************************************************************************
* Name: Yuehao Li
* Cruz ID: yli509
* CSE013E
* README.txt for Lab9
*********************************************************************************/
1. Name: Yuehao Li

I choose the solo option, I wrote:
-Agent.c
-Negotiation.c
-Message.c                                   
-MessageTest.c
-AgentTest.c

I did this lab by myself. 

The lab contains four parts, although I didn't choose to do Field.c, I can generally 
understand the way it works. The Field.c contains two fields, the own field, and the
opponent field. There are functions to mark each attack either from the opponent or 
the player. And because arguments are all pointers, we can easily modify field struct
in functions. The Message.c is used to change a special action into a string and change
a string into a specific order. The Negotiation.c checks whether the connection between
two players is safe and stable by using the hash. And agent.c is like a small main function to 
connect the other three files together to run the program. 

In Agentrun, there still have some bugs. Except this, the Message.c part works well. In 
the test harness, I implement the possible input step by step to find the possible error, which is 
useful.  

I learned how to write a complete program from this lab. I noticed that there are tons of 
state machines and structs need to deal with in a complete project. It provides a perfect 
chance to practice all material I learn in the past few weeks.   

This Lab is harder and more complex than any other lab. To be honest, I don't really like this lab, 
because the connection of files is complex and I need to spend lots of time to find out the usage 
of a simple function. But if I have more time, I will finish another half of the modules. 
