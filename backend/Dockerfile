# Utiliser une image Python légère
FROM python:3.9

# Définir le répertoire de travail
WORKDIR /app

# Copier les fichiers nécessaires
COPY . .

# Installer les dépendances
RUN pip install fastapi uvicorn

# Exposer le port 8000 pour l’API
EXPOSE 8000

# Commande pour lancer le serveur
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]
