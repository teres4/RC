import subprocess
import os

def generate_reports():
    # Define the ranges of numbers
    ranges = list(range(1, 12)) + list(range(21, 25))
    
    # Define the folder where reports will be saved
    reports_folder = "reports"
    
    # Create the folder if it doesn't exist
    os.makedirs(reports_folder, exist_ok=True)
    
    for num in ranges:
        # Format the number as two digits (e.g., 01, 02, ...)
        xx = f"{num:02d}"

        # Define the file path for the report
        file_path = os.path.join(reports_folder, f"report_{xx}.html")
        
        # Construct the command to send data to nc
        command = f'echo "172.20.48.1 58013 {xx}" | nc tejo.tecnico.ulisboa.pt 59000'
        
        try:
            # Open the file in write mode and capture the command output
            with open(file_path, "w") as output_file:
                subprocess.run(command, shell=True, stdout=output_file, check=True)

            print(f"Report for {xx} saved to {file_path}")

        except subprocess.CalledProcessError:
            print(f"Failed to generate report for {xx}")

# Run the function
if __name__ == "__main__":
    generate_reports()
