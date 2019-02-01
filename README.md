# Readme

## Q1

### newPerson

- We have a **newPerson** thread which creates a player or a refree based on the prbablities at random intervals of time o fless than 3 seconds.

### playerCreate
- When a player is created a global variable is incremented and the new value is set in a local variable of the thread which represents the **player number**.
- Then the player enters the academy represented by a print statement.
- Then player meets organiser (through **meetOrganiser** function) and it waits with the organiser until he/she aquires the mutex lock, a team of 3 is formed and he/she gets the signal that he/she is part of the team.
- Then the player warms up represented by a print statement and sleep(1).
- Then increment the player ready count for the team. If it equals to 2 i.e. the number of players needed is ready it signals the refree waiting for them and the game is starts.

### refreeCreate
- When a refree is created a global variable is incremented and the new value is set in a local variable of the thread which represents the **refree number**.
- Then the refree enters the academy represented by a print statement.
- Then refree meets organiser (through **meetOrganiser** function) and he/she waits with the organiser until he/she aquires the mutex lock, a team of 3 is formed and he/she gets the signal that he/she is part of the team.
- Then the refree adjust the equipment represented by a print statement and sleep(0.5).
- Wait for the players to get ready and start the game.


### How to run
- **gcc q1.c -pthread**
- **./a.out**
- **enter a number**


## Q2

### q2_declarations.c

- **struct booth**
    - The booth struct has it's number.
    - Has 1 mutex lock and 2 mutex condition variables.


- **polling_ready_evm**
    - Prints that evm is ready for voters and number of slots it has free.
    - Signals voters until either slots are over or all voters are done.
    - Waits for voter to get signal and get into a slot.
    - In case all voters done prints that all voter of that booth done and signals all evms so all of them get out of mutex lock.


- **voter_wait_for_evm**
    - Waits until a slot is free i.e. it catches a signal from evm.
    - stores the evm number which signalled.
    - decreases the total number of voters waiting.
    - returns evm number to voter thread.


- **voter_in_slot**
    - Voter in slot represented by a print statement.
    - Signals evm that it's in slot now.


- **booth_init**
    - Initializes booth with booth number.
    - Also initializes mutex locks and condition variables

### q2.c

- **evmStart**
    - Gives evm a number.
    - Finds it booth according to variable passed.
    - Calls polling_ready_evm until all voters done.
    - exits


- **voterStart**
    - Gives voter a number
    - Finds it booth according to variable passed.
    - Calls voter_wait_for_evm and takes the evm number which signalled.
    - Calls voter in slot to indicaate voter is voting.
    - exits

### How to run
- **gcc q2.c -pthread**
- **./a.out**
- **enter number of booths n**
- **n lines with number of voters and number of evms for each booth**

## Q3

### How to run
- **gcc q2.c -pthread**
- **./a.out**
- **enter a number n**
- **followed by n numbers to sort**

### Output
- merge1.txt has output of normal merge sort.
- merge2.txt has output of merge sort with processes.
- merge3.txt has output of merge sort with threads.
- Command line shows time taken by each merge.
