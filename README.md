# Rendu 3D ASCII en Temps Réel

## Présentation du Projet

Ce projet implémente un moteur de rendu 3D en temps réel utilisant des caractères ASCII pour l'affichage dans un terminal. L'application est capable de charger des modèles 3D au format GLB (Binary glTF) et de les afficher sous forme d'animation rotative dans un environnement console, démontrant une compréhension approfondie des concepts de graphiques 3D, d'optimisation algorithmique et de programmation système.

## Architecture et Conception

### Vue d'ensemble du système

Le projet est structuré selon une architecture modulaire composée de quatre modules principaux :

1. **Module de parsing (`parser.c/h`)** : Responsable du chargement et de la déduplication des données géométriques
2. **Module de rastérisation (`raster.c/h`)** : Implémente l'algorithme de remplissage de triangles avec gestion de la profondeur
3. **Module principal (`vertex.c`)** : Coordonne les transformations 3D et le pipeline de rendu
4. **Script d'automatisation (`maj.py`)** : Facilite la compilation et l'exécution du projet

## Utilisation et Configuration

### Compilation et Exécution

Le projet peut être compilé et exécuté via le script Python fourni :

```bash
python3 maj.py
```

Le script compile automatiquement les modules C et lance l'exécution avec le modèle par défaut.

### Configuration pour un Rendu Optimal

Pour obtenir un rendu cohérent et adapté aux différents modèles 3D, plusieurs paramètres peuvent être ajustés dans `vertex.c` :

#### 1. Ajustement de la Distance Caméra

La variable `D` (ligne ~15) contrôle la distance de la caméra par rapport au modèle :

```c
float D = 4; // Distance caméra - à ajuster selon le modèle
```

**Symptômes et solutions** :
- **Modèle trop petit/invisible** : Réduire la valeur de D (ex: D = 2 ou D = 1)
- **Modèle englobant la caméra** : Augmenter la valeur de D (ex: D = 6 ou D = 8)

#### 2. Contrôle des Rotations

Les vitesses de rotation peuvent être modifiées à la fin de la boucle de rendu (lignes ~80-82) :

```c
rotA += 0.02;  // Rotation axe X
rotB += 0.03;  // Rotation axe Y
rotC += 0.01;  // Rotation axe Z (commenté par défaut)
```

**Personnalisation** :
- Modifier les valeurs pour ajuster la vitesse d'animation
- Activer/désactiver la rotation sur l'axe Z selon les besoins
- Utiliser des valeurs négatives pour inverser le sens de rotation

## Environnement de Développement

**Compilation** : GCC avec liens dynamiques vers cgltf
**Dépendances** : 
- cgltf (parsing GLB)
- Bibliothèques mathématiques standard (libm)

### Structures de données

```c
typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    Vertex* unique_vertices;
    size_t unique_vertices_count;
    unsigned int* indices;
    size_t indices_count;
} GLBData;
```

Ces structures optimisent l'utilisation mémoire en éliminant la redondance des vertices partagés entre triangles.

## Algorithmes Implémentés

### 1. Pipeline de Rendu 3D

Le projet implémente un pipeline de rendu complet suivant les étapes classiques :

**Transformation des vertices** :
- Rotations 3D selon les trois axes (X, Y, Z) utilisant les matrices de rotation
- Application des transformations : R(x) × R(y) × R(z) × V

**Projection perspective** :
```c
screenX = centerX + scale * (worldX / (distance - worldZ))
screenY = centerY + scale * (worldY / (distance - worldZ))
```

**Rastérisation** :
- Algorithme de remplissage de triangles par scan-line avec interpolation linéaire
- Test de profondeur (Z-buffer) pour la visibilité des surfaces

### 2. Optimisations Géométriques

**Déduplication des vertices** :
Le système implémente un algorithme de déduplication utilisant une tolérance epsilon (10^-5) pour identifier les vertices identiques, réduisant significativement l'empreinte mémoire et les calculs redondants.

**Culling et clipping** :
- Vérification des limites d'écran avant rastérisation
- Évitement du rendu des triangles hors champ

### 3. Modèle d'Éclairage

Le rendu utilise un modèle d'éclairage de Phong simplifié :

```c
float dot = -(Nx*Lx + Ny*Ly + Nz*Lz);
float baseIntensity = (dot + 1.0f) * 0.4f + 0.2f;
```

- Calcul des normales par produit vectoriel
- Éclairage directionnel avec correction gamma
- Mappage des intensités sur une palette de caractères ASCII

## Aspects Techniques Avancés

### Gestion de la Mémoire

Le projet démontre une gestion rigoureuse de la mémoire :
- Allocation dynamique avec vérification d'erreurs
- Libération explicite des ressources via `free_glb_data()`
- Utilisation de buffers statiques pour les opérations temps-réel

### Intégration de Bibliothèques

**cgltf** : Intégration de la bibliothèque cgltf pour le parsing des fichiers GLB, démontrant la capacité à intégrer des bibliothèques tierces et à comprendre les formats de fichiers standards de l'industrie.

**Format GLB/glTF** : Support du format glTF 2.0, standard industriel pour les modèles 3D, incluant :
- Parsing des accesseurs et buffers
- Extraction des primitives de maillage
- Gestion des attributs de vertices

### Optimisations Performance

**Précalcul trigonométrique** :
```c
float sinA = sin(rotA), cosA = cos(rotA);
float sinB = sin(rotB), cosB = cos(rotB);
float sinC = sin(rotC), cosC = cos(rotC);
```

**Arithmétique en virgule flottante optimisée** :
- Utilisation de constantes epsilon pour éviter la division par zéro
- Optimisation des calculs de projection par factorisation

## Complexité Algorithmique

- **Parsing** : O(n) où n est le nombre de vertices
- **Déduplication** : O(n²) dans le pire cas, optimisable avec des structures de données avancées
- **Rendu par frame** : O(t) où t est le nombre de triangles
- **Rastérisation** : O(p) où p est le nombre de pixels par triangle

## Défis Techniques Résolus

### 1. Gestion de la Cohérence Visuelle
- Implémentation d'un Z-buffer pour résoudre les problèmes de visibilité
- Gestion des artefacts de rastérisation par interpolation bilinéaire

### 2. Optimisation des Performances
- Réduction des calculs redondants par précalcul
- Minimisation des appels système par buffering

### 3. Robustesse Numérique
- Gestion des cas limites (division par zéro, vertices colinéaires)
- Utilisation de tolérances appropriées pour les comparaisons flottantes
