const importAll = require =>
  require.keys().reduce((acc, next) => {
    acc[next.replace("./", "").split('.')[0]] = require(next).default;
    return acc;
  }, {});

const images = importAll(require.context('./', false, /\.(png|jpe?g|svg)$/));

export default images;

