!function(){"use strict";var a=window.document,b={STYLES:"https://c.disquscdn.com/next/recommendations/styles/recommendations.10022a97346f1c6e3798931bbd8e4bb5.css",RTL_STYLES:"https://c.disquscdn.com/next/recommendations/styles/recommendations_rtl.9c3ccad48c24340222b5a85c20ea36b8.css","recommendations/main":"https://c.disquscdn.com/next/recommendations/recommendations.bundle.2502c0f5aa4270464aad7e38352f9a52.js","remote/config":"https://disqus.com/next/config.js"};window.require={baseUrl:"https://c.disquscdn.com/next/current/recommendations/recommendations",paths:["recommendations/main","remote/config"].reduce(function(a,c){var d=-3;return a[c]=b[c].slice(0,d),a},{})};var c=a.createElement("script");c.onload=function(){require(["common/main"],function(a){a.init("recommendations",b)})},c.src="https://c.disquscdn.com/next/recommendations/common.bundle.ee9c33b24a56672a5987fadb46fbba34.js",a.body.appendChild(c)}();