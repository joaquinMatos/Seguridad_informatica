proveedores = []

def agregar_proveedor(nombre, contacto):
    proveedor = {"nombre": nombre, "contacto": contacto}
    proveedores.append(proveedor)

def obtener_proveedores():
    return proveedores
