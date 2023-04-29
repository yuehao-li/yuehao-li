#-------------------------------------------------------------------------
# Created by:  Li, Yuehao
#              yli509
#              2 Jun 2019
#
# Assignment:  Lab 5: A Gambling Game
#              CMPE 012, Computer Systems and Assembly Language
#              UC Santa Cruz, Spring 2019
# 
# Description: ---
# 
# Notes:       ---
#-------------------------------------------------------------------------


#--------------------------------------------------------------------
# play_game
#
# This is the highest level subroutine.
#
# arguments:  $a0 - starting score
#             $a1 - address of array index 0 
#
# return:     n/a
# REGISTER USE
# $a1   temp store address of array
# $a0   temp store score

#--------------------------------------------------------------------

.text
play_game: nop
    subi   $sp,  $sp,  8
    
                    # use $a1 to get the number of elements in the array.
    # move   $s5 , $a1                # use $s5 to store initial point of the array.
    sw     $a1, ($sp)
    
    
    #--------------------------------
    #addiu $a0  $zero  score_       # hard-coding the argument of array size for welcome subroutine.
                                   # DELETE THIS LINE AFTER IMPLEMENTING CODE TO GET ARRAY SIZE.
    
    #--------------------------------
    # move   $s4, $a0                # Store score in $s4.
    sw     $a0, 4($sp)
    
    
    # move   $a0,  $s5               # Array address as argument.
    lw     $a0,  ($sp)
    jal    get_array_size
    
    # move   $s3,  $v0              # Store array size in $t3.
    sw     $v0,  8($sp)
    
    # move   $a0,  $s3              # Array size as argument.
    lw     $a0,  8($sp)
    
    jal   welcome
    
    outside:
       # move   $a0,  $s5               # Array address as argument.
        lw     $a0,  ($sp)
        
        jal    get_array_size
       # move   $s3,  $v0              # Store array size in $t3.
        sw     $v0,  8($sp)
        
        jal   prompt_options
        #  some code
       
        move  $a3,  $v0             # $a3 to store selection as argument
       # move  $a1,  $s5             # Beginning address as argument
        lw    $a1,  ($sp)
        
       # move  $a0,  $s4             # Current score aas argument
        lw    $a0,  4($sp)
        
       # move  $a2,  $s3             # Array size as argument
        lw    $a2,  8($sp)
        jal   take_turn
        
       # move  $s4,  $v0             # Change updated score. 
        sw    $v0,  4($sp)
        j     outside
    # some code
    
    jal   end_game
    
    jr    $ra


#--------------------------------------------------------------------
# welcome (given)
#
# Prints welcome message indicating valid indices.
# Do not modify this subroutine.
#
# arguments:  $a0 - array size in words
#
# return:     n/a
#--------------------------------------------------------------------
#
# REGISTER USE
# $t0: array size
# $a0: syscalls
# $v0: syscalls
#--------------------------------------------------------------------

.data
welcome_msg: .ascii "------------------------------"
             .ascii "\nWELCOME"
             .ascii "\n------------------------------"
             .ascii "\n\nIn this game, you will guess the index of the maximum value in an array."
             .asciiz "\nValid indices for this array are 0 - "

end_of_msg:  .asciiz ".\n\n"
             
.text
welcome: nop

    add   $t0  $zero  $a0         # save address of array

    addiu $v0  $zero  4           # print welcome message
    la    $a0  welcome_msg
    syscall
   
    
    addiu $v0  $zero  1           # print max array index
    sub   $a0  $t0    1
    syscall

    addiu $v0  $zero  4           # print period
    la    $a0  end_of_msg
    syscall
    
    jr $ra
    
    
#--------------------------------------------------------------------
# prompt_options (given)
#
# Prints user options to screen.
# Do not modify this subroutine. No error handling is required.
# 
# return:     $v0 - user selection
#--------------------------------------------------------------------
#
# REGISTER USE
# $v0, $a0: syscalls
# $t0:      temporarily save user input
#--------------------------------------------------------------------

.data
turn_options: .ascii  "------------------------------" 
              .ascii  "\nWhat would you like to do? Select a number 1 - 3"
              .ascii  "\n"
              .ascii  "\n1 - Make a bet"
              .ascii  "\n2 - Cheat! Show me the array"
              .asciiz "\n3 - Quit before I lose everything\n\n"

.text
prompt_options: nop

    addiu $v0  $zero  4           # print prompts
    la    $a0  turn_options       
    syscall

    addiu $v0  $zero  5           # get user input
    syscall
    
    add   $t0  $zero  $v0         # temporarily saves user input to $t0
    
    
    addiu $v0  $zero  11
    addiu $a0  $zero  0xA         # print blank line
    syscall

    add   $v0  $zero  $t0         # return player selection
    jr    $ra


#--------------------------------------------------------------------
# take_turn	
#
# All actions taken in one turn are executed from take_turn.
#
# This subroutine calls one of following sub-routines based on the
# player's selection:
#
# 1. make_bet
# 2. print_array
# 3. end_game
#
# After the appropriate option is executed, this subroutine will also
# check for conditions that will lead to winning or losing the game
# with the nested subroutine win_or_lose.
# 
# arguments:  $a0 - current score
#             $a1 - address of array index 0 
#             $a2 - size of array (this argument is optional)
#             $a3 - user selection from prompt_options
#
# return:     $v0 - updated score
#--------------------------------------------------------------------
#
# REGISTER USE
# $a0: syscalls
# $v0: syscalls
# $t9    Array address
# $t8    Current Score
# $t7    Array size
#--------------------------------------------------------------------

.text
take_turn: nop
    move   $t9,  $a1             # Address in $t9
    move   $t8,  $a0             # Score in $t8
    move   $t7,  $a2             # Array size in $t7
    
    subi   $sp   $sp  4          # push return addres to stack
    sw     $ra  ($sp)

    beq    $a3,  1,  bet         # Checking user choice.
    beq    $a3,  2,  arrayprint
    beq    $a3,  3,  quit
    
    bet:
        move   $a1,  $t9            # move the initial point to $a1
        move   $a0,  $t8            # move the score to $a0
        jal    make_bet
        move   $t8,  $v0            # Updated score
        
        j      check
    arrayprint:
        move   $a0,  $t9            # address in $a0
        jal    print_array
        j      check
        
    quit:
        jal    end_game    
        
    check:         
        move   $a0,  $t9             # address to $a0
        move   $a1,  $t8             # score in $a1
        jal    win_or_lose
    
    move  $v0,  $t8             # Return updated score.
    lw    $ra  ($sp)            # pop return address from stack
    addi  $sp   $sp   4
    
    #--------------------------------

    #--------------------------------
        
    jr $ra


#--------------------------------------------------------------------
# make_bet
#
# Called from take_turn.
#
# Performs the following tasks:
#
# 1. Player is prompted for their bet along with their index guess.
# 2. Max value in array and index of max value is determined.
#    (find_max subroutine is called)
# 3. Player guess is compared to correct index.
# 4. Score is modified
# 5. If player guesses correctly, max value in array is either:
#    --> no extra credit: replaced by -1
#    --> extra credit:    removed from array
#  
# arguments:  $a0 - current score of user
#             $a1 - address of first element in array
#
# return:     $v0 - updated score
#--------------------------------------------------------------------
#
# REGISTER USE
# $a0: syscalls
# $v0: syscalls
# $t5       current score
# $t4       Array address
# $t3       Array size
# $t1       Max value
#--------------------------------------------------------------------


.data
bet_header:   .ascii  "------------------------------"
              .asciiz "\nMAKE A BET\n\n"
            
score_header: .ascii  "------------------------------"
              .asciiz "\nCURRENT SCORE\n\n"
pts:          .ascii  " pts\n\n\0"
            
# add more strings

.text
make_bet: nop      
    subi   $sp   $sp  4
    sw     $ra  ($sp)
    
    move   $t5,  $a0               # current score in $t0
    move   $t4,  $a1               # address in $t4
    
    move   $a0,  $t4
    jal    get_array_size
    move   $t3,  $v0              # Array size in $t3

    
    addiu  $v0  $zero  4           # print header
    la     $a0  bet_header
    syscall
    
    move   $a0,  $t4               # $a0 store start point.
    jal    find_max                # find max value.

    move   $t1,  $v0               # Index of max value.
    
    move   $a2,  $t3                # Array size as argument.
    move   $a1,  $t4                # Array index
    move   $a0,  $t5                # Score
    jal    prompt_bet

    move   $t2,  $v0             # Bet amount.
    
    move   $a0,  $v1             # User guess move to $a0 as argument.
    move   $a1,  $t1             # max index.
    jal    compare
    move   $t3,  $v0             # Compare result in $t3.
    
    
    move   $a0,  $t3             # Boolean value as argument
    move   $a1,  $v1             # User guess index.
    move   $a2,  $t2             # Bet amount as argument
    jal    print_result
    
    move   $a0,  $t5            # Current score as argument.
    move   $a1,  $t2            # Bet value as argument.
    move   $a2,  $t3            # Boolean value. ( Result of the comparison)
    jal    mod_score
    move   $t5,  $v0            # Updated score in $t5.
    
    li     $a0,  0
    la     $a0,  score_header
    li     $v0,  4
    syscall
    
    move   $a0, $t5
    li     $v0,  1
    syscall
    
    la     $a0,  pts
    li     $v0,  4
    syscall
    
    beq    $t3,  1 , mod
    j      back
    mod:
        move   $a0,  $t4            # Array address as argument.
        move   $a1,  $t1            # Max value index as  argument
        jal    mod_array
        j      back
        
    back:
        move   $v0,  $t5            # Return value in $v0 which is updated score.
        lw     $ra  ($sp)
        addi   $sp   $sp  4
        jr     $ra


#--------------------------------------------------------------------
# find_max
#
# Finds max element in array, returns index of the max value.
# Called from make_bet.
# 
# arguments:  $a0 - address of first element in array
#
# returns:    $v0 - index of the maximum element in the array
#             $v1 - value of the maximum element in the array
#--------------------------------------------------------------------
#
# REGISTER USE
# $a0: syscalls
# $v0: syscalls
# $v0    index max value
# $v1    max value
# $t0    store temp number in array
# $t1    store temp index for numbers
#--------------------------------------------------------------------

.text
find_max: nop
    li $v0,  0                 # initial index
    li $v1,  0                 # initial value
    li $t1,  0                 
    loop:
        lw    $t0 , ($a0)
        add   $a0,  $a0,  4
        beqz  $t0,  jumpback
        bgt   $t0,  $v1, setmax
        add   $t1,  $t1,   1
        j     loop
    
    setmax:
        add   $v1,  $zero, $t0
        move  $v0,  $t1
        
        add   $t1,  $t1,   1
        j     loop
        
    jumpback:

        jr     $ra        
         
    # some code
    


    


#--------------------------------------------------------------------
# win_or_lose
#
# After turn is taken, checks to see if win or lose conditions
# have been met
# 
# arguments:  $a0 - address of the first element in array
#             $a1 - updated score
#
# return:     n/a
#--------------------------------------------------------------------
#
# REGISTER USE
# $a0: syscalls
# $v0: syscalls
# $a1    score
# $t1    each element in array
#--------------------------------------------------------------------

.data
win_msg:  .ascii   "------------------------------"
          .asciiz  "\nYOU'VE WON! HOORAY! :D\n\n"

lose_msg: .ascii   "------------------------------"
          .asciiz  "\nYOU'VE LOST! D:\n\n"

.text
win_or_lose: nop
    beqz    $a1,  lose                    # If score is 0, jump to lose part. 
    j       winorlose
    winorlose:
        lw    $t1, ($a0)
        add   $a0,  $a0,  4
        beq   $t1,  -1,  winorlose
        beqz  $t1, win
        jr    $ra 
    # some code
    win:
        addiu  $v0  $zero  4
        la     $a0  win_msg
        syscall
        jal    end_game
    # some code
    lose:
        addiu  $v0  $zero  4
        la     $a0  lose_msg
        syscall
        jal    end_game



#--------------------------------------------------------------------
# print_array
#
# Print the array to the screen. Called from take_turn
# 
# arguments:  $a0 - address of the first element in array
#--------------------------------------------------------------------
#
# REGISTER USE
# $a0: syscalls
# $v0: syscalls
# $t0  array address
# $t1  array index
# $t2  Temp to store wrod in array
#--------------------------------------------------------------------

.data
cheat_header: .ascii  "------------------------------"
              .asciiz "\nCHEATER!\n\n"

.text
print_array: nop

    # some code
    move   $t0,  $a0               # store the address to t0.
    li     $t1,  0                 # array index
    addiu  $v0  $zero  4           # print header
    la     $a0  cheat_header
    syscall
    
    print:
        lw    $t2,  ($t0)           # Load word from array
        add   $t0,  $t0,  4         
        beqz  $t2,  arrayback 
        
        la    $a0,  ($t1)           # Array index
        li    $v0,  1
        syscall
        
        li    $a0,  58              # ':' character
        li    $v0,  11
        syscall
        
        li    $a0,  32             # Blank space
        syscall 
        
        la    $a0,  ($t2)         # Actual number of the array.
        li    $v0,   1
        syscall
        
        li    $a0,  10           # start from a new line.
        li    $v0,  11
        syscall
        
        add   $t1,  $t1,  1      # index plus 1
        j     print
    # some code
    arrayback:
        li    $a0,  10
        li    $v0,  11
        syscall
        
        jr    $ra


#--------------------------------------------------------------------
# end_game (given)
#
# Exits the game. Invoked by user selection or if the player wins or
# loses.
#
# arguments:  $a0 - current score
#
# returns:    n/a
#--------------------------------------------------------------------
#
# REGISTER USE
# $a0: syscalls
# $v0: syscalls
#--------------------------------------------------------------------

.data
game_over_header: .ascii  "------------------------------"
                  .ascii  " GAME OVER"
                  .asciiz " ------------------------------"

.text
end_game: nop

    add   $s0  $zero  $a0              # save final score

    addiu $v0  $zero  4                # print game over header
    la    $a0  game_over_header
    syscall
    
    addiu $v0  $zero  11               # print new line
    addiu $a0  $zero  0xA
    syscall
    
    addiu $v0  $zero  10               # exit program cleanly
    syscall


#--------------------------------------------------------------------
# OPTIONAL SUBROUTINES
#--------------------------------------------------------------------
# You are permitted to delete these comments.

#--------------------------------------------------------------------
# get_array_size (optional)
# 
# Determines number of 1-word elements in array.
#
# argument:   $a0 - address of array index 0
#
# returns:    $v0 - number of 1-word elements in array
# REGISTER USE
# $a0: syscalls
# $v0: syscalls
# $t1  temp number storage.
#--------------------------------------------------------------------
get_array_size: nop
    li    $v0,  0            # initial array size.
    size:
        lw    $t1,  ($a0)
        add   $a0,  $a0,  4
        beqz  $t1,  sizeback
        add   $v0,  $v0,  1
        j     size
    sizeback:
        jr    $ra
        
#--------------------------------------------------------------------
# prompt_bet (optional)
#
# Prompts user for bet amount and index guess. Called from make_bet.
# 
# arguments:  $a0 - current score
#             $a1 - address of array index 0
#             $a2 - array size in words
#
# returns:    $v0 - user bet
#             $v1 - user index guess

# REGISTER USE
# $a0: syscalls
# $v0: syscalls
# $t0  current score
# $t3   user bet
#--------------------------------------------------------------------
.data
prompt_valid:    .ascii "\nValid indices for this array are 0 -  \0"
prompt_which:    .ascii "Which index do you believe contains the maximum value? \0"
currently_have: .ascii "You currently have \0"
points:         .ascii " points.\nHow many points would you like to bet? \0"
exceed:         .ascii "\nSorry, your bet exceeds your current worth.\n\n\0 "
.text
prompt_bet: nop
     move     $t0,  $a0             # store current score in $t0.
     readint:
        la    $a0,  currently_have # print out question.
        li    $v0,  4              
        syscall 
        
        la    $a0, ($t0)              
        li    $v0,  1
        syscall
        
        la   $a0, points          # asking user bet number.
        li   $v0,   4
        syscall
        
        li    $v0,  5                 # read integer
        syscall
        
        bltz $v0,  errormessage
        bgt  $v0,  $t0, errormessage
        move $t3,  $v0               # Tempearey store user bet at $t3
        
        la   $a0,  prompt_valid
        li   $v0,    4
        syscall
        
        sub  $a2, $a2, 1               # Change to index
        la   $a0, ($a2)                # print max index number.
        li   $v0,  1 
        syscall
        
        li   $a0,  46               # '.' char
        li   $v0,  11
        syscall
        
        li   $a0,  10              # new line
        syscall
        
        la   $a0, prompt_which     # Print question to ask which index does the user want. 
        li   $v0,   4
        syscall
        
        li   $v0,  5              # Get users index.
        syscall
        
        move $v1,  $v0           # Finally $v1 is index that user guessed
        move $v0,  $t3           # $v0 is user's bet
        jr   $ra
        
    errormessage:
        la   $a0,  exceed           # print error message.
        li   $v0,  4
        syscall
        j    readint
    
    
#--------------------------------------------------------------------
# compare (optional)
#
# Compares user guess with index of largest element in array. Called
# from make_bet.
#
# arguments:  $a0 - player index guess
#             $a1 - index of the maximum element in the array
#
# return:     $v0 - 1 = correct guess, 0 = incorrect guess

# REGISTER USE
# $a0: syscalls
# $v0: syscalls

#--------------------------------------------------------------------
compare: nop
    beq    $a0,  $a1,  correct
    j      wrong
    correct:
        li    $v0,  1
        jr    $ra
    wrong:
        li    $v0,  0   
        jr    $ra
        
#--------------------------------------------------------------------
# mod_score (optional)
#
# Modifies score based on outcome of comparison between user guess
# correct answer. Returns score += bet for correct guess. Returns
# score -= bet for incorrect guess. Called from make_bet.
# 
# arguments:  $a0 - current score
#             $a1 - player’s bet
#             $a2 - boolean value from comparison
#
# return:     $v0 - updated score

# REGISTER USE
# $a0: syscalls
# $v0: syscalls
# $a2  boolean value 0 or 1
# $a1  user's bet.
#--------------------------------------------------------------------

mod_score: nop
    beq   $a2,  1,  true
    j     false
    true:
        add   $v0,  $a0,  $a1
        jr    $ra
    false:
        sub   $v0,  $a0,  $a1
        jr    $ra
        
#--------------------------------------------------------------------
# mod_array (optional)
#
# Replaces largest element in array with -1 if player guessed correctly.
# Called from make_bet.
#
# If extra credit implemented, the largest element in the array is
# removed and array shrinks by 1 element. Index of largest element
# is replaced by another element in the array.
# 
# arguments:  $a0 - address of array index 0
#             $a1 - index of the maximum element in the array
# 
# return:     n/a

# REGISTER USE
# $a0: syscalls
# $v0: syscalls
# $t1  index to add
#--------------------------------------------------------------------


.text
mod_array: nop
    mul    $t1,  $a1,  4
    add    $a0,  $t1,  $a0
    
    move_array:
        lw    $t1,   ($a0)
        beq   $t1,   0,   mod_back
        lw    $t1,  4($a0)
        sw    $t1,  ($a0)
        add   $a0,  $a0,  4
        j     move_array
    mod_back:
        jr   $ra
    #mul    $t1,  $a1,   4             # No extra credit, change it to -1
    #add    $a0,  $t1,  $a0
    #lw     $t0,  negative
    #sw     $t0, ($a0)
    #jr     $ra

# 
# $a0 - boolean.
# $a1 - user index
# $a2 - user bet.

.data
incorrect:    .ascii  "\nYour guess is incorrect! The maximum value is not in index \0"
lost:         .ascii  "\n\nYou lost \0"
earn:         .ascii  "You earned \0"
point:        .ascii  " Points! \n\n\0"
poi:          .asciiz " Points.\n\n"
remove:       .ascii  "This value has been removed from the array.\n\n\0"
score:        .ascii  "\nScore! Index \0"
max:          .ascii  " has the maximum value in the array.\n\n\0"
blank:        .ascii  ".\n\0"
.text
print_result: nop
    beq    $a0,  0,  minu
    beq    $a0,  1,  plus
    minu:
        la    $a0,  incorrect         # print String 
        li    $v0,  4
        syscall
        
        move  $a0,  $a1               # Print user index
        li    $v0,  1
        syscall        
        
        la    $a0,  46                # Character "."
        li    $v0,  11
        syscall  
        
        la    $a0,  lost              # Print lost points
        li    $v0,  4
        syscall
        
        move  $a0,  $a2
        li    $v0,  1
        syscall
        
        la    $a0,  poi
        li    $v0,  4
        syscall
        
        jr    $ra
    plus:
        la    $a0,  score           # Correct string
        li    $v0,  4
        syscall
        
        move  $a0,  $a1             # User index
        li    $v0,   1
        syscall
        
        la    $a0,  max
        li    $v0,  4
        syscall
        
        la    $a0,  earn
        syscall
        
        move  $a0,  $a2            # User bet
        li    $v0,  1 
        syscall
        
        la    $a0,  point
        li    $v0,   4
        syscall
        
        la    $a0,  remove        # Print has been removed
        syscall
        jr    $ra
