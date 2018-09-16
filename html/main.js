
var colors = {
  hex: '#194d33',
  hsl: { h: 150, s: 0.5, l: 0.2, a: 1 },
}

var device_ip= "192.168.0.52";
var ledid= "-1";
var RGB_channels=3;
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
		RGB_channels
	 }
  },
  computed: {
	 // a computed getter
	 url: function () {
		//http://192.168.0.52/fixrgb?r=400&g=4000&b=0&ledid=-1
		//http://192.168.0.100/fixhsl?h=0&s=1000&l=500
		//return "http://" + this.device_ip + "/fixhsl?h=" + Math.round(this.colors.hsl.h*1000/360) + "&s=" + Math.round(this.colors.hsl.s*1000) + "&l=" + Math.round(this.colors.hsl.l*1000) +"&ledid=" +this.ledid;
		return "http://" + this.device_ip + "/fixrgb?r=" + Math.round(this.colors.rgba.r*.588*4096/256) + "&g=" + Math.round(this.colors.rgba.g*4096/256) + "&b=" + Math.round(this.colors.rgba.b*.390*4096.0/256.0) +"&ledid=" +this.ledid;
	 },
	 url_off: function() {
		return "http://" + this.device_ip + "/off";
	 }
  },
  watch: {
	 colors: {
		handler: function(newColor, oldColor) {
		const Http = new XMLHttpRequest();
		Http.open("GET", this.url);
		Http.send();
		},
		deep: true
	 }
  },
	methods: {
		off: function () {
			const Http = new XMLHttpRequest();
			Http.open("GET", this.url_off);
			Http.send();
		}
	}
  
})