# INE5425 - Otimizador

# Introdução

Este projeto foi desenvolvido como parte da disciplina de Modelagem e Simulação do Curso de Ciências da Computação na Universidade Federal de Santa Catarina do semestre 2021/1.

Essa aplicação pode ser utilizada para otimizar variáveis de controle de modelos de simulação, sendo aplicadas restrições e tentando máximizar/minimizar uma função objetivo. Ele é construído para funcionar com o simulador [GenESys](https://github.com/rlcancian/2019_2022_GenESyS) mas com alguns ajustes pode ser utilizado com outros simuladores.

## TODO

- [X] 1. Read command
    - [X] 1.1. Parse command line
    - [X] 1.2. Check input file existance

- [ ] 2. Setup stuff
    - [ ] 2.1. Initialize OptimizationCore
    - [ ] 2.2. Check configurations integrity

- [ ] 3. Run simulation
    - [ ] 3.1. Create model input variations
    - [ ] 3.2. Split simulator calls in threads
    - [ ] 3.3. Make calls to the simulator
    - [ ] 3.4. Wait for threads to join

- [ ] 4. Read results
    - [ ] 4.1. Check existance for all reports
    - [ ] 4.2. Split parser calls in threads
    - [ ] 4.3. Parse reports and fill response vectors
    - [ ] 4.4. Wait for threads to join

- [ ] 5. Decide on optimization or end process (then goto 6 or 7)
    - [ ] 5.1. Check constraints of each OptimizationUnit
    - [ ] 5.2. Calculate fit for each OptimizationUnit
    - [ ] 5.3. Compare fit with solutions vector
    - [ ] 5.4. Check for pause instruction - [ ] GOTO 7.1
    - [ ] 5.5. Check for stop conditions - [ ] GOTO 7.1
    - [ ] 5.6. Continue optimization if there's no stop reason - [ ] GOTO 6.1 

- [ ] 6. Optimize
    - [ ] 6.1. Get two best fit from generation
    - [ ] 6.2. Mix control values
    - [ ] 6.3. Mutate some values randomly
    - [ ] 6.4. Run next generation simulations - [ ] GOTO 3.1

- [ ] 7. Post optimization interations
    - [ ] 7.1. Show stop reason and optimization results
    - [ ] 7.2. Check for optimization resume - [ ] GOTO 6.1
    - [ ] 7.3. Write results
    - [ ] 7.4. Close application