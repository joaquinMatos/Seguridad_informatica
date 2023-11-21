descuentos = []

def agregar_descuento(nombre_pintura, porcentaje):
    descuento = {"nombre_pintura": nombre_pintura, "porcentaje": porcentaje}
    descuentos.append(descuento)

def obtener_descuento(nombre_pintura):
    for descuento in descuentos:
        if descuento['nombre_pintura'] == nombre_pintura:
            return descuento['porcentaje']
    return 0
