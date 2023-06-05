import random
import csv

def generate_random_digits(n):
    return [random.randint(-500, 500) for _ in range(n)]

random_digits = generate_random_digits(1000000)

# Specify the file path and name
file_name = "random_digits.csv"

# Write the random digits to the CSV file
with open(file_name, 'w', newline='') as csv_file:
    writer = csv.writer(csv_file, delimiter=' ')
    writer.writerow(random_digits)

print("Random digits saved to", file_name)
