FROM eclipse-temurin:21-jdk
VOLUME /tmp
COPY target/chessmate-server-0.0.1-SNAPSHOT.jar app.jar
ENTRYPOINT ["java", "-jar", "/app.jar"]