/**
 * CUS (Control Unit Subsystem) - Componente principale del backend IoT
 *
 * Questa classe implementa un server HTTP che:
 * - Si connette a un broker MQTT per ricevere dati dai sensori
 * - Espone un'API REST per la memorizzazione dei dati
 * - Monitora la connettività MQTT e segnala se i messaggi mancano per troppo tempo
 *
 * @author Laboratorio IoT
 * @version 1.1
 */
package esiot.backend;

public class CUS {
    public static void main(String[] args) throws Exception {
        CUSService.main(args);
    }
}