# Arquitectura (MVC + Pipeline)

## Modelo (visioncore)
- `ImagePipeline`: almacena *artifacts* y ejecuta operaciones.
- `Operation`: interfaz común para cualquier transformación.
- `ParamSpec`: describe parámetros para auto-generar UI.

## Vista (visioncore_app.view)
- `MainWindow`: shell principal (menú, panel de herramientas, status bar).
- `VisionCanvas`: canvas multi-imagen con drag + selección.

## Controladores (visioncore_app.controller)
- `AppController`: traduce eventos UI ↔ Modelo y mantiene consistencia.

## Encadenamiento
Cada operación:
1) Toma el `active_id`.
2) Produce un nuevo `ImageArtifact`.
3) Se inserta en el canvas como nueva capa y se vuelve el activo.

Esto permite que la salida de un módulo sea la entrada del siguiente sin reiniciar la app.
