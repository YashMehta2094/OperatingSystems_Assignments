# Automated Backup Script

This shell script performs an automated backup of files from a source directory to a destination directory. The backup only includes files with names containing at least one vowel (a, e, i, o, u). The script also preserves the directory structure from the source directory in the backup location and logs the process statistics in a specified output file.

## Features

- **Backup Files with Vowels:** The script only backs up files that have at least one vowel in their name.
- **Preserve Directory Structure:** The directory structure from the source is maintained in the destination.
- **Logging:** The script logs the PID, runtime (in milliseconds), and the number of files copied to a CSV output file.
- **Efficient Backups:** Only files that have changed since the last backup are copied.

## Usage

### Running the Script

To run the script, use the following command:

```bash
./backup_script.sh -s <source_directory> -d <destination_directory> -o <output_file>
```

<source_directory>: The directory containing the files you want to back up.

<destination_directory>: The directory where the backup will be stored. If this directory doesn’t exist, the script will create it.

<output_file>: The file where the backup statistics will be logged. If this file doesn’t exist, the script will create it.

```bash
./backup_script.sh -s ~/Documents/srcfolder -d ~/MyFolder/mybackup -o ~/Desktop/backup_stats.csv

In this example:

Files from ~/Documents/srcfolder are backed up to ~/MyFolder/mybackup.

The statistics (PID, runtime, files copied) are logged in ~/Desktop/backup_stats.csv.
```


## Setting Up a Cronjob
To automate the backup process to run every day at 12 midnight, you can set up a cronjob in your Linux system.

Steps to Set Up Cronjob

1. Open the crontab file for editing: crontab -e

2. Add the following line to schedule the script to run at midnight every day:

        0 0 * * * /path/to/backup_script.sh -s /path/to/source_directory -d /path/to/destination_directory -o /path/to/output_file.csv

        Replace /path/to/backup_script.sh, /path/to/source_directory, /path/to/destination_directory, and /path/to/output_file.csv with the actual paths on your system.

3. Save and exit the editor:

    If you’re using nano, press Ctrl + O to save and Ctrl + X to   exit.
    If you’re using vim, press Esc, then type :wq, and press Enter.

4. Verify the cronjob is set: crontab -l

5. For example:

        0 0 * * * /home/user/Desktop/backup_script.sh -s /home/user/Documents/srcfolder -d /home/user/MyFolder/mybackup -o /home/user/Desktop/backup_stats.csv

        This cronjob will run the backup_script.sh every day at 12:00 AM, backing up files and logging the statistics.


# Notes
Ensure the script has execute permissions:
        
        chmod +x backup_script.sh

The script is designed to be portable and should work on any standard Linux distribution.