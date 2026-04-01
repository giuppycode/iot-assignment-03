package esiot.backend;

import io.vertx.core.Vertx;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.client.WebClient;

public class CUSService {
    static final int PORT = 8080;
    static final long TIMEOUT_MS = 3000;

    public static void main(String[] args) throws Exception {
        Vertx vertx = Vertx.vertx();
        DataService service = new DataService(PORT);
        vertx.deployVerticle(service);

        WebClient client = WebClient.create(vertx);
        HTTPProtocol httpProtocol = new HTTPProtocol(client, PORT);

        MQTTProtocol mqttProtocol = new MQTTProtocol();
        mqttProtocol.connect(payload -> {
            try {
                float distanza = Float.parseFloat(payload.trim());
                httpProtocol.sendData(distanza, "tank");
            } catch (NumberFormatException e) {
                System.out.println("[MQTT] Payload non numerico: " + payload);
            }
        });

        SerialProtocol serialProtocol = new SerialProtocol();
        serialProtocol.initialize(new SerialProtocol.SerialCallback() {
            @Override
            public void onModeChanged(String mode) {
                httpProtocol.sendMode(mode);
            }

            @Override
            public void onValveChanged(int valve) {
                httpProtocol.sendValve(valve);
            }
        });
        serialProtocol.sendMode("AUTOMATIC");

        vertx.setPeriodic(1000, id -> {
            long elapsed = System.currentTimeMillis() - mqttProtocol.getLastReceivedTime();

            httpProtocol.getStatus(new HTTPProtocol.Callback<String>() {
                @Override
                public void onSuccess(String apiMode) {
                    boolean timedOut = elapsed > TIMEOUT_MS;
                    String targetMode = (timedOut || "UNCONNECTED".equals(apiMode))
                            ? "UNCONNECTED" : apiMode;

                    if (timedOut) {
                        System.out.println("[MQTT] Status: unreachable ("
                                + elapsed / 1000 + "s without data)");
                    } else {
                        System.out.println("[MQTT] Status: connected, mode=" + targetMode);
                    }

                    serialProtocol.sendMode(targetMode);

                    if (!serialProtocol.getCurrentMode().equals(targetMode)) {
                        System.out.println("[SERIAL] Pushed mode to Arduino: "
                                + serialProtocol.getCurrentMode() + " → " + targetMode);
                    }

                    if ("AUTOMATIC".equals(targetMode)) {
                        httpProtocol.getData(new HTTPProtocol.Callback<JsonArray>() {
                            @Override
                            public void onSuccess(JsonArray arr) {
                                if (arr == null || arr.isEmpty()) return;

                                double level = arr.getJsonObject(0).getDouble("value");

                                final double L1 = 40.0;
                                final double L2 = 20.0;

                                int autoValve;
                                if (level <= L2) {
                                    autoValve = 100;
                                } else if (level <= L1) {
                                    autoValve = 50;
                                } else {
                                    autoValve = 0;
                                }

                                httpProtocol.sendValve(autoValve);
                            }

                            @Override
                            public void onFailure(String error) {
                                System.out.println("[HTTP] Failed to get data: " + error);
                            }
                        });
                    }
                }

                @Override
                public void onFailure(String error) {
                    System.out.println("[HTTP] Failed to get status: " + error);
                }
            });
        });
    }
}
