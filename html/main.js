
var colors = {
  hex: '#194d33',
  hsl: { h: 150, s: 0.5, l: 0.2, a: 1 },
}
var balance_whites=true;
var white_adjust ={
	r:739,
	g:1000,
	b:350
}
var device_ip= "192.168.0.52";
var mode="rainbow";
var cycles= 100;
var ledid= "-1";
var RGB_channels=3;
var color_array = { "c0" : "#ddd","c1" : "#ddd","c2" : "#ddd" }
console.log(Vue.version)
var app = new Vue({
  el: '#app',
  components: {
	 'photoshop-picker': window.VueColor.Photoshop,
	 'material-picker': window.VueColor.Material,
	 'compact-picker': window.VueColor.Compact,
	 'swatches-picker': window.VueColor.Swatches,
	 'slider-picker': window.VueColor.Slider,
	 'sketch-picker': window.VueColor.Sketch,
	 'chrome-picker': window.VueColor.Chrome,
  },
  data () {
	 return {
		colors,
		device_ip,
		ledid,
		RGB_channels,
		balance_whites,
		white_adjust,
		cycles,
		mode,
		color_array
	 }
  },
  computed: {
	 // a computed getter
	 url: function () {
		//http://192.168.0.52/fixrgb?r=400&g=4000&b=0&ledid=-1
		//http://192.168.0.100/fixhsl?h=0&s=1000&l=500
		//return "http://" + this.device_ip + "/fixhsl?h=" + Math.round(this.colors.hsl.h*1000/360) + "&s=" + Math.round(this.colors.hsl.s*1000) + "&l=" + Math.round(this.colors.hsl.l*1000) +"&ledid=" +this.ledid;
		if(this.balance_whites){
			var r=Math.round(this.colors.rgba.r*this.white_adjust.r/1000*4096/256);
			var g=Math.round(this.colors.rgba.g*this.white_adjust.g/1000*4096/256);
			var b=Math.round(this.colors.rgba.b*this.white_adjust.b/1000*4096.0/256.0);
		}
		else{
			var r=Math.round(this.colors.rgba.r*4096/256);
			var g=Math.round(this.colors.rgba.g*4096/256);
			var b=Math.round(this.colors.rgba.b*4096.0/256.0);	
		}
		return "http://" + this.device_ip + "/fixrgb?r=" + r + "&g=" + g + "&b=" + b +"&ledid=" +this.ledid;
	 },
	 url_off: function() {
		return "http://" + this.device_ip + "/off";
	 },
	 url_mode: function() {
		return "http://" + this.device_ip + "/mode?palette="+ this.mode + "&ledid=" + this.ledid+ "&cycles=" + Math.round(Math.pow(2,this.cycles/10.0));
	 },
  },
  watch: {
	 colors: {
		handler: function(newColor, oldColor) {
			if (this.ledid == -1) {
				this.color_array.c0 = newColor.hex;
				this.color_array.c1 = newColor.hex;
				this.color_array.c2 = newColor.hex;
			}
			else if (this.ledid == 0) {this.color_array.c0 = newColor.hex;}
			else if (this.ledid == 1) {this.color_array.c1 = newColor.hex;}
			else if (this.ledid == 2) {this.color_array.c2 = newColor.hex;}
			this.send_command(this.url);
		},
		deep: true
	 }
  },
	methods: {
		off: function () {
			this.send_command(this.url_off);
		},
		send_command(url_command){
			const Http = new XMLHttpRequest();
			Http.open("GET", url_command);
			Http.send();
		},
		dumpwhites: function() {
			this.send_command(this.url)
		},
		mode_rainbow: function(){
			this.mode = "rainbow";

			if (this.ledid == -1 || this.ledid == 0) {this.color_array.c0 = 'url(#linear)';}
			else if (this.ledid == -1 || this.ledid == 1) {this.color_array.c1 = 'url(#linear)';}
			else if (this.ledid == -1 || this.ledid == 2) {this.color_array.c2 = 'url(#linear)';}
			this.send_command(this.url_mode)
		},
		mode_random: function(){
			this.mode = "random";

			if (this.ledid == -1 || this.ledid == 0) {this.color_array.c0 = 'url(#linear)';}
			else if (this.ledid == -1 || this.ledid == 1) {this.color_array.c1 = 'url(#linear)';}
			else if (this.ledid == -1 || this.ledid == 2) {this.color_array.c2 = 'url(#linear)';}

			this.send_command(this.url_mode);

		},
		refresh_cycles: function () {
			this.send_command(this.url_mode);
		},
		setLedId: function (id) {
			this.ledid=id;
		},
		panelSelectColor: function (id) {
			if(this.ledid==id || this.ledid ==-1){
				return "#dfd";
			}
			return "#ddd"
		},

		panelColor: function (id) {
			if (id == 0) {return this.color_array.c0 }
			else if (id == 1) {return this.color_array.c1 }
			else if (id == 2) {return this.color_array.c2 }
			else return "#000";
		},

	}
  
})