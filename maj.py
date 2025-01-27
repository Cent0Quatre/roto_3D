import os
import subprocess

def compile_and_run(c_file, c_file_2, c_file_3, output_file, model):
    try:
        # Supprimer le fichier .exe s'il existe
        if os.path.exists(output_file):
            os.remove(output_file)
            print(f"\nFichier {output_file} supprimé.")
        
        # Compiler le fichier .c
        compile_command = f"gcc {c_file} {c_file_2} {c_file_3} -o {output_file}"
        compile_process = subprocess.run(compile_command, shell=True, text=True, capture_output=True)
        
        if compile_process.returncode != 0:
            print("Erreur lors de la compilation:")
            print(compile_process.stderr)
            return
        
        print("\nCompilation réussie")
        
        # Exécuter le fichier compilé
        print("\nExécution du programme: ")
        run_command = f"./{output_file} {model}" if os.name != 'nt' else output_file, model
        subprocess.run(run_command, shell=True)
    
    except Exception as e:
        print(f"\nErreur : {e}")

# Fichier source et fichier de sortie
c_file = "vertex.c"
c_file_2 = "parser.c"
c_file_3 = "raster.c"
model = "./models/monk.glb"
output_file = "vertex.exe"

compile_and_run(c_file, c_file_2, c_file_3, output_file, model)
