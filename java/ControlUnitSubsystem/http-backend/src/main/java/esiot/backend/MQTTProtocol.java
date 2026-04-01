package esiot.backend;

import java.util.concurrent.atomic.AtomicLong;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MQTTProtocol {
    private static final String BROKER = "tcp://broker.mqtt-dashboard.com";
    private static final String TOPIC = "esiot-2025";

    private final AtomicLong lastReceived = new AtomicLong(System.currentTimeMillis());
    private MqttClient mqttClient;

    public MQTTProtocol() {}

    public void connect(MQTTCallback callback) throws Exception {
        String clientId = "cus-" + System.currentTimeMillis();
        mqttClient = new MqttClient(BROKER, clientId);

        mqttClient.setCallback(new MqttCallback() {
            @Override
            public void messageArrived(String topic, MqttMessage message) {
                lastReceived.set(System.currentTimeMillis());

                String payload = new String(message.getPayload()).trim();
                System.out.println("[MQTT] Ricevuto: " + payload);

                callback.onMessageReceived(payload);
            }

            @Override
            public void connectionLost(Throwable cause) {
                System.out.println("[MQTT] Connessione persa: " + cause.getMessage());
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {}
        });

        mqttClient.connect();
        mqttClient.subscribe(TOPIC, 1);
        System.out.println("[MQTT] Iscritto al topic: " + TOPIC);
    }

    public long getLastReceivedTime() {
        return lastReceived.get();
    }

    public interface MQTTCallback {
        void onMessageReceived(String payload);
    }
}
