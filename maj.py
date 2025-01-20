import os
import subprocess

def compile_and_run(c_file, output_file):
    try:
        # Supprimer le fichier .exe s'il existe
        if os.path.exists(output_file):
            os.remove(output_file)
            print(f"\nFichier {output_file} supprimé.")
        
        # Compiler le fichier .c
        compile_command = f"gcc {c_file} -o {output_file}"
        compile_process = subprocess.run(compile_command, shell=True, text=True, capture_output=True)
        
        if compile_process.returncode != 0:
            print("Erreur lors de la compilation:")
            print(compile_process.stderr)
            return
        
        print("\nCompilation réussie")
        
        # Exécuter le fichier compilé
        print("\nExécution du programme: ")
        run_command = f"./{output_file}" if os.name != 'nt' else output_file
        subprocess.run(run_command, shell=True)
    
    except Exception as e:
        print(f"\nErreur : {e}")

# Fichier source et fichier de sortie
c_file = "donut.c"
output_file = "donut.exe"

compile_and_run(c_file, output_file)
