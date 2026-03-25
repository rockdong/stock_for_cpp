/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        buy: '#07C160',
        sell: '#FA5151',
        hold: '#888888',
      }
    },
  },
  plugins: [],
}