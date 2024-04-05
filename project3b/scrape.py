import requests

# Base URL and filenames to download
base_url = "https://www.cs.hmc.edu/courses/2023/fall/cs134/Samples/"
file_numbers = range(1, 23)
file_types = ['err', 'csv']

# Function to download a file
def download_file(url, file_name):
    response = requests.get(url)
    if response.status_code == 200:
        with open(file_name, 'wb') as f:
            f.write(response.content)
        print(f"Downloaded {file_name}")
    else:
        print(f"Failed to download {file_name}")

# Downloading files
for number in file_numbers:
    for file_type in file_types:
        file_name = f"P3B-test_{number}.{file_type}"
        url = base_url + file_name
        download_file(url, file_name)
