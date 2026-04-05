package esiot.backend;

import java.util.Date;
import java.util.LinkedList;

import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpMethod;
import io.vertx.core.http.HttpServerResponse;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import io.vertx.ext.web.handler.CorsHandler;

/*
 * Data Service as a vertx event-loop 
 */
public class DataService extends AbstractVerticle {

	private int port;
	private static final int MAX_SIZE = 10;
	private static final long TIMEOUT_MS = 3000;
	private LinkedList<DataPoint> values;
	private String mode = "AUTOMATIC";
	private float valvePercent = 0;
	private long lastReceived = System.currentTimeMillis();
	private WCSService wcsService;
	
	private static final float L1 = 0.20f;
	private static final float L2 = 0.30f;
	private static final long T1_MS = 2000;
	private float currentDistance = 0;
	private long l1ConditionStartTime = 0;
	private int currentValvePercent = -1;
	
	public DataService(int port) {
		values = new LinkedList<>();		
		this.port = port;
	}

	public void setWCSService(WCSService wcsService) {
		this.wcsService = wcsService;
	}

	public void setMode(String mode) {
		this.mode = mode;
		log("Mode updated to: " + mode);
		if ("MANUAL".equals(mode)) {
			l1ConditionStartTime = 0;
		}
	}

	public String getMode() {
		return mode;
	}

	public void setValvePercent(float percent) {
		this.valvePercent = percent;
		this.currentValvePercent = (int) percent;
		log("Valve percent updated to: " + percent);
	}

	public void onDistanceReceived(float distance) {
		this.currentDistance = distance;
		this.lastReceived = System.currentTimeMillis();
		
		if ("AUTOMATIC".equals(mode)) {
			applyAutomaticPolicy(distance);
		}
	}
	
	private void applyAutomaticPolicy(float distance) {
		int newValvePercent = 0;
		
		if (distance >= L2) {
			newValvePercent = 100;
			l1ConditionStartTime = 0;
		} else if (distance > L1) {
			long now = System.currentTimeMillis();
			if (l1ConditionStartTime == 0) {
				l1ConditionStartTime = now;
			} else if (now - l1ConditionStartTime > T1_MS) {
				newValvePercent = 50;
			}
		} else {
			l1ConditionStartTime = 0;
			newValvePercent = 0;
		}
		
		if (newValvePercent != currentValvePercent && newValvePercent != 0) {
			currentValvePercent = newValvePercent;
			valvePercent = newValvePercent;
			log("Automatic mode: setting valve to " + newValvePercent + "% (distance=" + distance + ")");
			if (wcsService != null) {
				wcsService.sendValve(newValvePercent);
			}
		} else if (newValvePercent == 0 && currentValvePercent != 0) {
			currentValvePercent = 0;
			valvePercent = 0;
			log("Automatic mode: closing valve (distance=" + distance + ")");
			if (wcsService != null) {
				wcsService.sendValve(0);
			}
		}
	}

	@Override
	public void start() {		
		Router router = Router.router(vertx);
		
		router.route().handler(CorsHandler.create("*")
				.allowedMethod(HttpMethod.GET)
				.allowedMethod(HttpMethod.POST)
				.allowedMethod(HttpMethod.OPTIONS)
				.allowedHeader("Content-Type"));
		
		router.route().handler(BodyHandler.create());
		router.post("/api/data").handler(this::handleAddNewData);
		router.get("/api/data").handler(this::handleGetData);	
		router.get("/api/status").handler(this::handleGetStatus);
		router.post("/api/status").handler(this::handleSetStatus);
		router.get("/api/valve").handler(this::handleGetValve);
		router.post("/api/valve").handler(this::handleSetValve);

		vertx
			.createHttpServer()
			.requestHandler(router)
			.listen(port);

		log("Service ready on port: " + port);
	}
	
	private void handleAddNewData(RoutingContext routingContext) {
		HttpServerResponse response = routingContext.response();
		JsonObject res = routingContext.getBodyAsJson();
		if (res == null) {
			sendError(400, response);
		} else {
			float value = res.getFloat("value");
			String place = res.getString("place");
			long time = System.currentTimeMillis();
			
			values.addFirst(new DataPoint(value, time, place));
			if (values.size() > MAX_SIZE) {
				values.removeLast();
			}
			lastReceived = time;
			
			log("New value: " + value + " from " + place + " on " + new Date(time));
			
			onDistanceReceived(value);
			
			response.setStatusCode(200).end();
		}
	}
	
	private void handleGetData(RoutingContext routingContext) {
		JsonArray arr = new JsonArray();
		for (DataPoint p: values) {
			JsonObject data = new JsonObject();
			data.put("time", p.getTime());
			data.put("value", p.getValue());
			data.put("place", p.getPlace());
			arr.add(data);
		}
		routingContext.response()
			.putHeader("content-type", "application/json")
			.putHeader("Access-Control-Allow-Origin", "*")
			.end(arr.encodePrettily());
	}
	
	private void sendError(int statusCode, HttpServerResponse response) {
		response.setStatusCode(statusCode).end();
	}

	private void log(String msg) {
		System.out.println("[DATA SERVICE] "+msg);
	}

	private void handleGetStatus(RoutingContext ctx) {
    JsonObject obj = new JsonObject();
    String effectiveMode = mode;
    if (mode.equals("AUTOMATIC") && System.currentTimeMillis() - lastReceived > TIMEOUT_MS) {
        effectiveMode = "UNCONNECTED";
    }
    obj.put("mode", effectiveMode);
    ctx.response()
        .putHeader("content-type", "application/json")
        .putHeader("Access-Control-Allow-Origin", "*")
        .end(obj.encodePrettily());
}

private void handleSetStatus(RoutingContext ctx) {
    JsonObject body = ctx.getBodyAsJson();
    if (body == null) { sendError(400, ctx.response()); return; }
    String newMode = body.getString("mode", mode);
    if (!newMode.equals(mode)) {
        mode = newMode;
        log("Mode changed to: " + mode);
        if (wcsService != null) {
            wcsService.sendMode(mode);
        }
    }
    ctx.response().putHeader("Access-Control-Allow-Origin", "*")
       .setStatusCode(200).end();
}

private void handleGetValve(RoutingContext ctx) {
    JsonObject obj = new JsonObject();
    obj.put("percent", valvePercent);
    ctx.response()
        .putHeader("content-type", "application/json")
        .putHeader("Access-Control-Allow-Origin", "*")
        .end(obj.encodePrettily());
}

private void handleSetValve(RoutingContext ctx) {
    JsonObject body = ctx.getBodyAsJson();
    if (body == null) { sendError(400, ctx.response()); return; }
    valvePercent = body.getFloat("percent", valvePercent);
    log("Valve set to: " + valvePercent + "%");
    if (wcsService != null) {
        wcsService.sendValve((int) valvePercent);
    }
    ctx.response().putHeader("Access-Control-Allow-Origin", "*")
       .setStatusCode(200).end();
}
}