pinturas = []

def agregar_pintura(nombre, color, precio, proveedor):
    pintura = {"nombre": nombre, "color": color, "precio": precio, "proveedor": proveedor}
    pinturas.append(pintura)

def obtener_pinturas():
    return pinturas
