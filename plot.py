import matplotlib.pyplot as plt

# Data for plotting
dataset_sizes = [500, 1000, 2000]
search_times = [100, 150, 200]  # in milliseconds
insertion_times = [50, 75, 100]  # in milliseconds
memory_usage = [50, 75, 100]  # in megabytes
accuracies = [90, 85, 95]  # in percentage

# Plot Search Time vs. Dataset Size
plt.figure(figsize=(8, 6))
plt.plot(dataset_sizes, search_times, marker='o')
plt.title('Search Time vs. Dataset Size')
plt.xlabel('Dataset Size')
plt.ylabel('Search Time (ms)')
plt.grid(True)
plt.show()

# Plot Insertion Time vs. Dataset Size
plt.figure(figsize=(8, 6))
plt.plot(dataset_sizes, insertion_times, marker='o')
plt.title('Insertion Time vs. Dataset Size')
plt.xlabel('Dataset Size')
plt.ylabel('Insertion Time (ms)')
plt.grid(True)
plt.show()

# Plot Memory Usage vs. Dataset Size
plt.figure(figsize=(8, 6))
plt.plot(dataset_sizes, memory_usage, marker='o')
plt.title('Memory Usage vs. Dataset Size')
plt.xlabel('Dataset Size')
plt.ylabel('Memory Usage (MB)')
plt.grid(True)
plt.show()

# Plot Accuracy
plt.figure(figsize=(8, 6))
plt.bar(['Scenario 1', 'Scenario 2', 'Scenario 3'], accuracies)
plt.title('Accuracy of Search Results')
plt.xlabel('Scenario')
plt.ylabel('Accuracy (%)')
plt.ylim(0, 100)
plt.grid(axis='y')
plt.show()
