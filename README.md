# DualFader (VST3)

Petite table de mixage : l'entrée stéréo est dupliquée en deux pistes stéréo
indépendantes, chacune avec son fader, puis re-sommées vers un bus stéréo
avec un fader master.

```
Entrée stéréo ─┬─► Piste 1 ─► Fader 1 ─┐
               │                        ├─► Somme ─► Fader Master ─► Sortie stéréo
               └─► Piste 2 ─► Fader 2 ─┘
```

- Faders de -inf (en dessous de -60 dB) à +12 dB, par pas de 0,1 dB
- Double-clic sur un fader : retour à 0 dB
- Gains lissés (20 ms) pour éviter les clics
- Paramètres automatisables dans la DAW et sauvegardés avec le projet

## Compilation

Prérequis : CMake 3.22+, git, un compilateur C++17
(Visual Studio 2022 sur Windows, Xcode sur macOS, GCC/Clang sur Linux).

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

JUCE est téléchargé automatiquement lors de la première configuration.
Avec `COPY_PLUGIN_AFTER_BUILD`, le .vst3 est copié dans le dossier VST3 standard
de ton système. Sinon, tu le trouveras dans
`build/DualFader_artefacts/Release/VST3/`.

Sous Windows, si la copie échoue pour cause de droits, lance la compilation
depuis un terminal administrateur ou copie le .vst3 à la main dans
`C:\Program Files\Common Files\VST3`.

## Étendre le plugin

Dans `Source/PluginProcessor.cpp`, le commentaire « C'est ici que tu pourras
insérer un traitement » marque l'endroit où ajouter un effet propre à chaque
piste (`track1` et `track2` contiennent alors chacune une copie du signal).
