#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WiFiUdp.h>


const char* ssid = "CLARO-77C0"; // Nombre de tu red Wi-Fi
const char* password = "Cl4r0@6777C0"; // Contraseña de tu red Wi-Fi

//const char* ssid = "Riwi Coders"; // Nombre de tu red Wi-Fi
//const char* password = "RiwyMed2o23&"; // Contraseña de tu red Wi-Fi
ESP8266WebServer server(80);

// Datos de conexión a la base de datos
const char* host = "b7j7mvgzxcjuzj2uxrtr-mysql.services.clever-cloud.com"; // Host de tu servidor MySQL en Clever Cloud
char* user = "uhgrdewymycewnsb"; // Usuario de la base de datos
char* passwordCLEVER = "MrpfiMCc54ApZqvsuldn"; // Contraseña de la base de datos
char* db = "b7j7mvgzxcjuzj2uxrtr"; // Nombre de la base de datos
const int port = 3306; // Puerto de MySQL


// Objeto para la conexión MySQL
WiFiClient client;
MySQL_Connection conn((Client *)&client);

void handleRoot() {

     if (LittleFS.exists("pagina.html")) {
        File file = LittleFS.open("pagina.html", "r");
        if (file) {
            // Envía el contenido del archivo al cliente
            server.streamFile(file, "text/html");
            file.close();
        } else {
            Serial.println("Error al abrir el archivo 'pagina.html'");
            server.send(500, "text/plain", "Error interno del servidor");
        }
    } else {
        Serial.println("El archivo 'pagina.html' no existe");
        server.send(404, "text/plain", "Archivo no encontrado");
    }

   
}


void handleGuardarDatos() {
  String documentoRecibido = server.arg("documento");
  String horaEntradaRecibida = server.arg("horaEntrada");
  String horaSalidaRecibida = server.arg("horaSalida");

  Serial.println("Datos recibidos:");
  Serial.println("Documento: " + documentoRecibido);
  Serial.println("Hora entrada: " + horaEntradaRecibida);
  Serial.println("Hora salida: " + horaSalidaRecibida);
  MySQL_Cursor cur_mem(&conn); // Mover la creación del cursor fuera del loop
  Serial.println("agregando datos de la tabla Registtos:");
  delay(100);
  String documento_usuario	 = documentoRecibido;
  String hora_ingreso_sql = horaEntradaRecibida;
  String hora_salida_sql = horaSalidaRecibida;
  String insert_query = "INSERT INTO Registros (documento_usuario, hora_ingreso, hora_salida) VALUES ('" + documento_usuario + "', '" + hora_ingreso_sql + "', '" + hora_salida_sql + "')";
  const char* insert_query_cstr = insert_query.c_str();
  if (cur_mem.execute(insert_query_cstr)) {
        Serial.println("Nuevo Registro agregado correctamente.");
        server.send(200, "application/json", "{\"mensaje\": \"Registro agregado correctamente\"}");
    } else {
        Serial.println("Error al agregar nuevo usuario.");
    }
  // Enviar una respuesta al cliente
  
}
void setup() {
  Serial.begin(9600);

  // Conéctate a la red Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red Wi-Fi...");
  }
    Serial.print("Conectado a la red Wi-Fi. Dirección IP: ");
  Serial.println(WiFi.localIP());
  // Inicia LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Error al montar LittleFS");
    return;
  }

  Serial.println("LittleFS montado correctamente");

  IPAddress resolvedIP;
  if (WiFi.hostByName(host, resolvedIP)) 
    {
        Serial.print("Dirección IP resuelta: ");
        Serial.println(resolvedIP);
    } 
  else 
    {
        Serial.println("Error en la búsqueda DNS.");
    }
  // Conexión a la base de datos MySQL
  Serial.println("Conectando a la base de datos MySQL...");

  if (conn.connect(resolvedIP, port, user, passwordCLEVER, db)) 
  {
    Serial.println("Conexión exitosa a la base de datos MySQL");
  }
  else 
  {
    Serial.println("Error de conexión a la base de datos MySQL");
    return;
  }

 // Configura la ruta para manejar la solicitud a la raíz "/"
  server.on("/", HTTP_GET, handleRoot);

  // Configura la ruta para manejar las solicitudes POST a "/registrar"
  server.on("/guardar-datos", HTTP_POST, handleGuardarDatos);

  // Inicia el servidor
  server.begin();
  Serial.println("Servidor iniciado");
}


   

void loop() {
  // Maneja las solicitudes de los clientes
  server.handleClient();
}