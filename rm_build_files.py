import os
import shutil

def hesitant_destroyer():
    # starting from here, because making decisions is hard
    cwd = os.getcwd()
    for root, dirs, files in os.walk(cwd, topdown=False):
        for name in dirs:
            if name == 'build':
                full_path = os.path.join(root, name)
                # asking for permission, because we're polite destructors
                confirm = input(f"Poised to delete '{full_path}'. May I? (y/n): ")
                if confirm.lower() == 'y':
                    # your wish is my command, oh flip-floppy one
                    shutil.rmtree(full_path)
                    print(f"Annihilated: {full_path}")
                else:
                    # saved by the bell, or in this case, your indecision
                    print(f"Mercifully spared: {full_path}")

hesitant_destroyer()

print("The purging is complete. Or is it? Check your y/n reflexes.")

