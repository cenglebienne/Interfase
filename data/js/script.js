var timerEventos;
var CAMPOS_CONFIG_LENGTH = 40;
var ssidTargetId = "ssidDestino";


function abrirTab(evt, tabName) {
	var i, tablinks, x;
	x = document.getElementsByClassName("tabContent");
	for (i = 0; i < x.length; i++) {
		x[i].style.display = "none";
	}
	document.getElementById(tabName).style.display = "block";

	tablinks = document.getElementsByClassName("nav-btn");
	for (i = 0; i < tablinks.length; i++) {
		tablinks[i].classList.remove("active");
	}
	evt.currentTarget.classList.add("active");

	if (evt.currentTarget.id == "barItemEstadoCliente") {
		clearInterval(timerEventos);
		recuperarEstadoCliente();
	} else if (evt.currentTarget.id == "barItemMQTT") {
		clearInterval(timerEventos);
		recuperarEstadoMQTT();
	} else if (evt.currentTarget.id == "barItemEquipo") {
		clearInterval(timerEventos);
		recuperarEstadoEquipo();
	} else if (evt.currentTarget.id == "barItemContadores") {
		clearInterval(timerEventos);
		recuperarEstadoContadores();
	} else if (evt.currentTarget.id == "barItemValores") {
		clearInterval(timerEventos);
		recuperarEstadoValores();
	} else if (evt.currentTarget.id == "barItemGeneral") {
		clearInterval(timerEventos);
	} else if (evt.currentTarget.id == "barItemAcerca") {
		clearInterval(timerEventos);
		recuperarEstadoAcerca();
	}
}

function recuperarEstadoCliente() {
	var xmlhttp = new XMLHttpRequest();
	var params = "noCache=" + new Date().getTime();
	var cantFilas;
	var tabla;
	var row;
	var cell1;
	var cell2;
	xmlhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			var myObj = JSON.parse(this.responseText);
			tabla = document.getElementById("tablaEstadoCliente");
			cantFilas = tabla.rows.length;
			for (i = 0; i < cantFilas; i++) {
				tabla.deleteRow(0);
			}
			for (j = 0; j < Object.keys(myObj).length; j++) {
				row = tabla.insertRow(tabla.rows.length);
				cell1 = row.insertCell(0);
				cell1.textContent = Object.keys(myObj)[j];
				cell2 = row.insertCell(1);
				cell2.textContent = Object.values(myObj)[j];
			}
		}
	};
	xmlhttp.open("GET", "estadoCliente.json?" + params, true);
	xmlhttp.send();
}

function recuperarEstadoMQTT() {
	var xmlhttp = new XMLHttpRequest();
	var params = "noCache=" + new Date().getTime();
	var cantFilas;
	var tabla;
	var row;
	var cell1;
	var cell2;
	xmlhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			var myObj = JSON.parse(this.responseText);
			tabla = document.getElementById("tablaEstadoMQTT");
			cantFilas = tabla.rows.length;
			for (i = 0; i < cantFilas; i++) {
				tabla.deleteRow(0);
			}
			for (j = 0; j < Object.keys(myObj).length; j++) {
				row = tabla.insertRow(tabla.rows.length);
				cell1 = row.insertCell(0);
				cell1.textContent = Object.keys(myObj)[j];
				cell2 = row.insertCell(1);
				cell2.textContent = Object.values(myObj)[j];
			}
		}
	};
	xmlhttp.open("GET", "estadoMQTT.json?" + params, true);
	xmlhttp.send();
}

function recuperarEstadoEquipo() {
	var xmlhttp = new XMLHttpRequest();
	var params = "noCache=" + new Date().getTime();
	var cantFilas;
	var tabla;
	var row;
	var cell1;
	var cell2;
	xmlhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			var myObj = JSON.parse(this.responseText);
			tabla = document.getElementById("tablaEstadoEquipo");
			cantFilas = tabla.rows.length;
			for (i = 0; i < cantFilas; i++) {
				tabla.deleteRow(0);
			}
			for (j = 0; j < Object.keys(myObj).length; j++) {
				row = tabla.insertRow(tabla.rows.length);
				cell1 = row.insertCell(0);
				cell1.textContent = Object.keys(myObj)[j];
				cell2 = row.insertCell(1);
				cell2.textContent = Object.values(myObj)[j];
			}
		}
	};
	xmlhttp.open("GET", "estadoEquipo.json?" + params, true);
	xmlhttp.send();
}

function recuperarEstadoContadores() {
	var xmlhttp = new XMLHttpRequest();
	var params = "noCache=" + new Date().getTime();
	var map = [
		{ newKey: "Cont_Parcial_Cospeles", label: "Parcial Fichas"  },
		{ newKey: "Cont_Total_Cospeles",   label: "Total Fichas"    },
		{ newKey: "Cont_Parcial_OnLine",   label: "Parcial OnLine"  },
		{ newKey: "Cont_Total_Online",     label: "Total OnLine"    }
	];
	xmlhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			var myObj = JSON.parse(this.responseText);
			var tabla = document.getElementById("tablaEstadoContadores");
			var cantFilas = tabla.rows.length;
			for (var i = 0; i < cantFilas; i++) tabla.deleteRow(0);
			for (var i = 0; i < map.length; i++) {
				var entry = map[i];
				var val = myObj[entry.newKey];
				var row = tabla.insertRow(tabla.rows.length);
				row.insertCell(0).textContent = entry.label;
				row.insertCell(1).textContent = (val !== undefined) ? val : "--";
			}
		}
	};
	xmlhttp.open("GET", "estadoContadores.json?" + params, true);
	xmlhttp.send();
}

function actualizarEspejo() {
	var esEspejo = document.getElementById("modoEspejo").checked;
	var deshabilitar = esEspejo || document.getElementById("modoTimer").value === "true";
	document.getElementById("salidaDurMs").disabled = deshabilitar;
	document.getElementById("row_salidaDurMs").classList.toggle("row-disabled", deshabilitar);

	// La cantidad de pulsos solo aplica sin espejo (en espejo se replica 1 a 1 lo que entra).
	document.getElementById("salidaCantidadPulsos").disabled = deshabilitar;
	document.getElementById("row_salidaCantidadPulsos").classList.toggle("row-disabled", deshabilitar);
}

function seleccionarModo(esTimer) {
	document.getElementById("modoTimer").value = esTimer ? "true" : "false";
	document.getElementById("segBtnInterface").classList.toggle("active", !esTimer);
	document.getElementById("segBtnTimer").classList.toggle("active",  esTimer);

	// Modo Espejo solo aplica en Interface
	document.getElementById("row_modoEspejo").classList.toggle("row-disabled", esTimer);
	document.getElementById("modoEspejo").disabled = esTimer;

	document.getElementById("tiempoProgramado").disabled = !esTimer;
	document.getElementById("row_tiempoProgramado").classList.toggle("row-disabled", !esTimer);

	actualizarEspejo();
}

function recuperarEstadoValores() {
	var xmlhttp = new XMLHttpRequest();
	var params = "noCache=" + new Date().getTime();
	xmlhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			var myObj = JSON.parse(this.responseText);

			// modoEspejo debe establecerse ANTES de seleccionarModo para que
			// actualizarEspejo() lea el valor correcto del JSON.
			var toBool = function(v) { return v === true || v === "true" || v === 1 || v === "1"; };
			document.getElementById("modoEspejo").checked = toBool(myObj["modoEspejo"]);

			// Modo de operación (llama actualizarEspejo internamente con modoEspejo ya correcto)
			var esTimer = (myObj["modoTimer"] === true || myObj["modoTimer"] === "true");
			seleccionarModo(esTimer);

			// Otros toggles booleanos
			var boolFields = ["entradaActivoAlto", "usarHabilitacionExterna", "salidaActivoAlto"];
			for (var i = 0; i < boolFields.length; i++) {
				document.getElementById(boolFields[i]).checked = toBool(myObj[boolFields[i]]);
			}

			// Campos numéricos
			var numFields = [
				{ id: "salidaDurMs",           estId: "est_salidaDurMs"           },
				{ id: "salidaCantidadPulsos",  estId: "est_salidaCantidadPulsos"  },
				{ id: "tiempoProgramado",      estId: "est_tiempoProgramado"      }
			];
			for (var j = 0; j < numFields.length; j++) {
				var v = myObj[numFields[j].id];
				document.getElementById(numFields[j].id).value = v;
				document.getElementById(numFields[j].estId).textContent = v;
			}
		}
	};
	xmlhttp.open("GET", "estadoValores.json?" + params, true);
	xmlhttp.send();
}

function recuperarEstadoAcerca() {
	var xmlhttp = new XMLHttpRequest();
	var params = "noCache=" + new Date().getTime();
	var cantFilas;
	var tabla;
	var row;
	var cell1;
	var cell2;
	xmlhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			var myObj = JSON.parse(this.responseText);
			tabla = document.getElementById("tablaEstadoAcerca");
			cantFilas = tabla.rows.length;
			for (i = 0; i < cantFilas; i++) {
				tabla.deleteRow(0);
			}
			for (j = 0; j < Object.keys(myObj).length; j++) {
				row = tabla.insertRow(tabla.rows.length);
				cell1 = row.insertCell(0);
				cell1.textContent = Object.keys(myObj)[j];
				cell2 = row.insertCell(1);
				cell2.textContent = Object.values(myObj)[j];
			}
		}
	};
	xmlhttp.open("GET", "estadoAcerca.json?" + params, true);
	xmlhttp.send();
}

function aplicarConfigCliente() {
	var ssidDestino    = document.getElementById("ssidDestino").value.trim();
	var clave_cliente  = document.getElementById("clave_cliente").value.trim();
	var ssidDestino2   = document.getElementById("ssidDestino2").value.trim();
	var clave_cliente2 = document.getElementById("clave_cliente2").value.trim();
	var habilitadoWiFi = document.getElementById("habilitadoWiFi").checked;

	var params = "ssidDestino="    + ssidDestino
		+ "&clave_cliente="    + clave_cliente
		+ "&ssidDestino2="     + ssidDestino2
		+ "&clave_cliente2="   + clave_cliente2
		+ "&habilitadoWiFi="   + habilitadoWiFi;

	if (habilitadoWiFi) {
		if (ssidDestino == "") {
			alert("El campo SSID no puede estar vacio, por favor verifique.");
			return false;
		}
		if (ssidDestino.length > CAMPOS_CONFIG_LENGTH || clave_cliente.length > CAMPOS_CONFIG_LENGTH
				|| ssidDestino2.length > CAMPOS_CONFIG_LENGTH || clave_cliente2.length > CAMPOS_CONFIG_LENGTH) {
			alert("Los campos no pueden tener un tamano mayor a " + CAMPOS_CONFIG_LENGTH + " caracteres");
			return false;
		}
	}
	var r = confirm("Se guardaran los cambios, desea continuar?");
	if (r == true) {
		var xmlhttp = new XMLHttpRequest();
		xmlhttp.open("POST", "grabarConfigCliente.php", true);
		xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		xmlhttp.onreadystatechange = function () {
			if (this.readyState == 4 && this.status == 200) {
				if (this.responseText == "ok") {
					document.getElementById("ssidDestino").value = "";
					document.getElementById("clave_cliente").value = "";
					document.getElementById("ssidDestino2").value = "";
					document.getElementById("clave_cliente2").value = "";
					document.getElementById("habilitadoWiFi").value = true;
					recuperarEstadoCliente();
					alert("La operacion fue exitosa. Recuerde reiniciar para aplicar los cambios.");
				} else {
					alert("Se produjo un error al realizar la operacion");
				}
			}
		};
		xmlhttp.send(params);
	}
	return false;
}

function defaultConfigMQTT() {
	document.getElementById("servidorMQTT").value = "66.97.47.118";
	document.getElementById("pathMQTT").value = "FarinaVending";
	document.getElementById("usuarioMQTT").value = "farinamqtt";
	document.getElementById("claveMQTT").value = "fv#297esp8266";
	return false;
}

function aplicarConfigMQTT() {
	var servidorMQTT = document.getElementById("servidorMQTT").value.trim();
	var pathMQTT = document.getElementById("pathMQTT").value.trim();
	var usuarioMQTT = document.getElementById("usuarioMQTT").value.trim();
	var claveMQTT = document.getElementById("claveMQTT").value.trim();

	var params = "servidorMQTT=" + servidorMQTT
		+ "&pathMQTT=" + pathMQTT
		+ "&usuarioMQTT=" + usuarioMQTT
		+ "&claveMQTT=" + claveMQTT;

	if (servidorMQTT == "" || pathMQTT == "") {
		alert("Solo los campos Usuario y Clave pueden estar vacios, por favor verifique.");
	} else if (servidorMQTT.length > CAMPOS_CONFIG_LENGTH || pathMQTT.length > CAMPOS_CONFIG_LENGTH) {
		alert("Los campos no pueden tener un tamano mayor a " + CAMPOS_CONFIG_LENGTH + " caracteres");
	} else {
		var r = confirm("Se guardaran los cambios, desea continuar?");
		if (r == true) {
			var xmlhttp = new XMLHttpRequest();
			xmlhttp.open("POST", "grabarConfigMQTT.php", true);
			xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
			xmlhttp.onreadystatechange = function () {
				if (this.readyState == 4 && this.status == 200) {
					if (this.responseText == "ok") {
						document.getElementById("servidorMQTT").value = "";
						document.getElementById("pathMQTT").value = "";
						document.getElementById("usuarioMQTT").value = "";
						document.getElementById("claveMQTT").value = "";
						recuperarEstadoMQTT();
						alert("La operacion fue exitosa. Recuerde reiniciar para aplicar los cambios.");
					} else {
						alert("Se produjo un error al realizar la operacion");
					}
				}
			};
			xmlhttp.send(params);
		}
	}
	return false;
}

function aplicarConfigContadores() {
	var Cont_Parcial_Cospeles = document.getElementById("Cont_Parcial_Cospeles").value.trim();
	var Cont_Total_Cospeles   = document.getElementById("Cont_Total_Cospeles").value.trim();
	var Cont_Parcial_OnLine   = document.getElementById("Cont_Parcial_OnLine").value.trim();
	var Cont_Total_Online     = document.getElementById("Cont_Total_Online").value.trim();

	var params = "Cont_Parcial_Cospeles=" + Cont_Parcial_Cospeles
		+ "&Cont_Total_Cospeles="  + Cont_Total_Cospeles
		+ "&Cont_Parcial_OnLine="  + Cont_Parcial_OnLine
		+ "&Cont_Total_Online="    + Cont_Total_Online;

	if (Cont_Parcial_Cospeles == "" || Cont_Total_Cospeles == ""
		|| Cont_Parcial_OnLine == "" || Cont_Total_Online == "") {
		alert("No puede existir ningun campo vacio, por favor verifique.");
	} else {
		var r = confirm("Se guardaran los cambios, desea continuar?");
		if (r == true) {
			var xmlhttp = new XMLHttpRequest();
			xmlhttp.open("POST", "grabarConfigContadores.php", true);
			xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
			xmlhttp.onreadystatechange = function () {
				if (this.readyState == 4 && this.status == 200) {
					if (this.responseText == "ok") {
						document.getElementById("Cont_Parcial_Cospeles").value = "0";
						document.getElementById("Cont_Total_Cospeles").value = "0";
						document.getElementById("Cont_Parcial_OnLine").value = "0";
						document.getElementById("Cont_Total_Online").value = "0";
						recuperarEstadoContadores();
						alert("La operacion fue exitosa. Recuerde reiniciar para aplicar los cambios.");
					} else {
						alert("Se produjo un error al realizar la operacion");
					}
				}
			};
			xmlhttp.send(params);
		}
	}
	return false;
}

function habilitarDeshabilitarWiFi() {
	var checkBox = document.getElementById("habilitadoWiFi");
	var ssidDestino    = document.getElementById("ssidDestino");
	var clave          = document.getElementById("clave_cliente");
	var ssidDestino2   = document.getElementById("ssidDestino2");
	var clave2         = document.getElementById("clave_cliente2");
	var botonSeleccionarAP  = document.getElementById("seleccionarAP");
	var botonSeleccionarAP2 = document.getElementById("seleccionarAP2");
	var en = checkBox.checked;
	ssidDestino.disabled        = !en;
	clave.disabled              = !en;
	ssidDestino2.disabled       = !en;
	clave2.disabled             = !en;
	botonSeleccionarAP.disabled  = !en;
	botonSeleccionarAP2.disabled = !en;
}

function aplicarConfigEquipo() {
	var serieEquipo = document.getElementById("serieEquipo").value.trim();
	var params = "serieEquipo=" + serieEquipo;

	if (serieEquipo == "") {
		alert("Ningun campo puede estar vacio, por favor verifique.");
	} else if (serieEquipo.length > CAMPOS_CONFIG_LENGTH) {
		alert("Los campos no pueden tener un tamano mayor a " + CAMPOS_CONFIG_LENGTH + " caracteres");
	} else {
		var r = confirm("Se guardaran los cambios, desea continuar?");
		if (r == true) {
			var xmlhttp = new XMLHttpRequest();
			xmlhttp.open("POST", "grabarConfigEquipo.php", true);
			xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
			xmlhttp.onreadystatechange = function () {
				if (this.readyState == 4 && this.status == 200) {
					if (this.responseText == "ok") {
						document.getElementById("serieEquipo").value = "";
						recuperarEstadoEquipo();
						alert("La operacion fue exitosa. Recuerde reiniciar para aplicar los cambios.");
					} else {
						alert("Se produjo un error al realizar la operacion");
					}
				}
			};
			xmlhttp.send(params);
		}
	}
	return false;
}

function aplicarConfigValores() {
	var esTimer              = document.getElementById("modoTimer").value === "true";
	var esEspejo             = document.getElementById("modoEspejo").checked;
	var salidaDurMs          = parseInt(document.getElementById("salidaDurMs").value);
	var salidaCantidadPulsos = parseInt(document.getElementById("salidaCantidadPulsos").value);
	var tiempoProgramado     = parseInt(document.getElementById("tiempoProgramado").value);

	if (!esTimer && isNaN(salidaDurMs)) {
		alert("La duracion de salida debe tener un valor valido.");
		return false;
	}
	if (!esTimer && !esEspejo && (isNaN(salidaCantidadPulsos) || salidaCantidadPulsos < 1)) {
		alert("La cantidad de pulsos debe ser un valor valido (minimo 1).");
		return false;
	}
	if (esTimer && isNaN(tiempoProgramado)) {
		alert("El tiempo de activacion debe tener un valor valido.");
		return false;
	}

	var params = "entradaActivoAlto=" + document.getElementById("entradaActivoAlto").checked
		+ "&usarHabilitacionExterna=" + document.getElementById("usarHabilitacionExterna").checked
		+ "&modoTimer="         + (esTimer ? "true" : "false")
		+ "&salidaDurMs="       + salidaDurMs
		+ "&salidaCantidadPulsos=" + (isNaN(salidaCantidadPulsos) ? 1 : salidaCantidadPulsos)
		+ "&tiempoProgramado="  + tiempoProgramado
		+ "&salidaActivoAlto="  + document.getElementById("salidaActivoAlto").checked
		+ "&modoEspejo="        + esEspejo;

	var r = confirm("Se guardaran los cambios, desea continuar?");
	if (r == true) {
		var xmlhttp = new XMLHttpRequest();
		xmlhttp.open("POST", "grabarConfigValores.php", true);
		xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		xmlhttp.onreadystatechange = function () {
			if (this.readyState == 4 && this.status == 200) {
				if (this.responseText == "ok") {
					recuperarEstadoValores();
					alert("La operacion fue exitosa.");
				} else {
					alert("Se produjo un error al realizar la operacion");
				}
			}
		};
		xmlhttp.send(params);
	}
	return false;
}

function reiniciar() {
	var r = confirm("El equipo se reiniciara y se aplicaran los cambios pendientes, desea continuar?");
	if (r == true) {
		var xmlhttp = new XMLHttpRequest();
		xmlhttp.open("POST", "reiniciar.php", true);
		xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		xmlhttp.onreadystatechange = function () {
			if (this.readyState == 4 && this.status == 200) {
				if (this.responseText == "ok")
					alert("La operacion fue exitosa");
				else
					alert("Se produjo un error al realizar la operacion");
			}
		};
		xmlhttp.send();
	}
	return false;
}

function cargarAPDisponibles(targetId) {
	ssidTargetId = targetId || "ssidDestino";
	var xmlhttp = new XMLHttpRequest();
	var params = "noCache=" + new Date().getTime();
	var cantFilas;
	var tabla;
	var row;
	var cell1;
	var cell2;
	var cell3;
	document.getElementById('modalAguarde').style.display = 'block';

	xmlhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			var myObj = JSON.parse(this.responseText);
			tabla = document.getElementById("tablaAPDisponibles");
			cantFilas = tabla.rows.length;
			for (i = 1; i < cantFilas; i++) {
				tabla.deleteRow(1);
			}
			for (j = 0; j < myObj.length; j++) {
				row = tabla.insertRow(tabla.rows.length);
				row.setAttribute('onclick', 'setSSIDDestino(this)');
				cell1 = row.insertCell(0);
				cell1.textContent = myObj[j].SSID;
				cell2 = row.insertCell(1);
				cell2.textContent = myObj[j].potencia;
				cell3 = row.insertCell(2);
				cell3.textContent = myObj[j].encriptacion;
			}
			document.getElementById('modalAguarde').style.display = 'none';
			document.getElementById('modalAPsDisponibles').style.display = 'block';
		}
	};

	xmlhttp.open("GET", "APdisponibles.json?" + params, true);
	xmlhttp.send();
	return false;
}

function setSSIDDestino(obj) {
	obj.style.backgroundColor = 'red';
	document.getElementById(ssidTargetId).value = obj.cells[0].textContent;
	document.getElementById('modalAPsDisponibles').style.display = 'none';
}
