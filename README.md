HexGame
=======

A Monte Carlo Tree Search implementation of Artificial Intelligence (AI) player in Hex Game
This is Command line version of Hex Game. User will be prompted to make a series of choices,
including choice of color (usually first play first win in Hex Game) and choice of AI opponent.

Current existing AI opponent choices:

Naive Monte Carlo implementation includes pure Monte Carlo AI player with simple heuristics  
Monte Carlo Tree Search implementation with Upper Confidence for Tree (UCT) with Upper Confidence Bounds (UCB) [1] 
Parallel Monte Carlo Tree Search implementation is a parallelized version of Monte Carlo Tree Search. 

### Make
* g++ >= 4.8.2
* make rel

### Run
./bin/HexBoardGameApp

### Additional Information
A UI interface for hexgame written by Python can be found under PyGameUI repository  

###Reference:
[1] Sylvain Gelly, David Silver (2007). "Combining Online and Offline Knowledge in UCT" Machine Learning, Proceedings of the Twenty-Fourth International conference (ICML 2007), Corvallis, Oregon, USA, June 20-24, 2007. Zoubin Ghahramani (ed.). ACM. pp.273-280. ISBN 978-1-59593-793-3
