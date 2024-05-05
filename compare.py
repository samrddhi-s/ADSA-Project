import matplotlib.pyplot as plt
import numpy as np

# Insertion and Searching times for B-tree
btree_insertion_times = [3,6,9]
btree_searching_times = [1.8,3.6,5.4]

# Insertion and Searching times for BST
bst_insertion_times = [3,6,9]
bst_searching_times = [2.2,4.4,6.6]

# Insertion and Searching times for AVL Tree
avl_insertion_times = [4.6,9.2,13.8]
avl_searching_times =  [2.2,4.4,6.6]

# Insertion and Searching times for splay tree
splay_insertion_times = [6,12,18]
splay_searching_times = [5,10,15]

# Number of scenarios
scenarios = np.arange(1, 4)

# Plotting insertion times
plt.figure(figsize=(10, 6))
plt.plot(scenarios, btree_insertion_times, marker='o', label='B-tree')
plt.plot(scenarios, bst_insertion_times, marker='o', label='BST')
plt.plot(scenarios, avl_insertion_times, marker='o', label='AVL Tree')
plt.plot(scenarios, splay_insertion_times, marker='o', label='Splay Tree')
plt.title('Insertion Times Comparison')
plt.xlabel('Scenario')
plt.ylabel('Number of steps in insertion')
plt.xticks(scenarios)
plt.legend()
plt.grid(True)
plt.show()

# Plotting searching times
plt.figure(figsize=(10, 6))
plt.plot(scenarios, btree_searching_times, marker='o', label='B-tree')
plt.plot(scenarios, bst_searching_times, marker='o', label='BST')
plt.plot(scenarios, avl_searching_times, marker='o', label='AVL Tree')
plt.plot(scenarios, splay_searching_times, marker='o', label='splay Map')
plt.title('Searching Times Comparison')
plt.xlabel('Scenario')
plt.ylabel('Number of steps in searching')
plt.xticks(scenarios)
plt.legend()
plt.grid(True)
plt.show()
