# Projet Fils Rouge — Pince à Mouvement Continu pour Robot Doosan A0509S

## Objectif du projet

Concevoir et intégrer une **pince à mouvement continu** contrôlée par un **moteur pas à pas avec driver TMC2130**, afin d’équiper le **robot manipulateur Doosan A0509S**.  

## Fonctionnement général

La pince est pilotée par une carte **Arduino** communiquant avec le **driver TMC2130** via SPI.  
Le moteur effectue un mouvement linéaire (vis sans fin) commandé en pas, avec détection de butée par un **capteur de contact (crash sensor)**.

### Séquence de fonctionnement

1. **Initialisation (`run`)**
   - Le moteur se déplace jusqu’au capteur de fin de course.
   - Cette position devient la **position zéro**.
   - Le système est ensuite prêt à recevoir des commandes.

2. **Commande de mouvement**
   - Commande vers une position précise : `gXXXX`
   - Ouverture complète : `open`
   - Fermeture complète : `close`
   - Ajustement manuel : `p+` / `p-`

3. **Contrôle du moteur**
   - `stop` → Arrête le moteur.
   - `resume` → Relance le mouvement en cours.

4. **Affichage Série**
   - Pendant le mouvement, la position actuelle et la cible sont affichées :
     ```
     Position: 1200, Cible: 1500, Mode: ouverture
     ```

## Points techniques importants

### Pilotage moteur (TMC2130)
| Commande | Rôle |
|-----------|------|
| `driver.rms_current(700)` | Définit le courant moteur → plus le courant est élevé, plus la pince tient fort |
| `driver.microsteps(8)` | Définit le nombre de micro-pas par pas complet → augmente la précision |
| `driver.en_pwm_mode(true)` & `driver.pwm_autoscale(true)` | Active la modulation PWM pour un mouvement fluide et silencieux |
| `driver.pwm_freq(1)` | Régle la fréquence PWM (1 = équilibré entre douceur et puissance) |

## Commandes série disponibles

| Commande | Description |
|-----------|-------------|
| `run` | Lance l’initialisation (détection du zéro) |
| `resume` | Relance après un stop |
| `stop` | Arrête le moteur |
| `open` | Ouvre la pince au maximum |
| `close` | Ferme la pince jusqu’à la butée |
| `gXXXX` | Va à la position spécifiée (ex. `g1500`) |
| `p+` | Avance de 100 pas |
| `p-` | Recule de 100 pas |


## Fichier

**`Code_Arduino_Pince_manipulatrice`**  
Contient le code complet de contrôle du moteur, des commandes série et de l’initialisation automatique.

