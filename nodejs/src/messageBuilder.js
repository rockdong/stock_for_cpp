function buildRichTextCard(title, sections, template = 'blue') {
  const elements = sections.map(section => {
    if (section.type === 'div') {
      return {
        tag: 'div',
        text: {
          tag: 'lark_md',
          content: section.content
        }
      };
    }
    if (section.type === 'hr') {
      return { tag: 'hr' };
    }
    return null;
  }).filter(Boolean);

  return {
    msg_type: 'interactive',
    card: {
      config: {
        wide_screen_mode: true
      },
      header: {
        title: {
          tag: 'plain_text',
          content: title
        },
        template: template
      },
      elements: elements
    }
  };
}

function buildInteractiveCard(title, content, buttons, template = 'blue') {
  const buttonElements = buttons.map(btn => ({
    tag: 'button',
    text: {
      tag: 'plain_text',
      content: btn.text
    },
    type: btn.type || 'primary',
    action_id: btn.action_id || 'default_action',
    value: JSON.stringify(btn.value || {})
  }));

  return {
    msg_type: 'interactive',
    card: {
      config: {
        wide_screen_mode: true
      },
      header: {
        title: {
          tag: 'plain_text',
          content: title
        },
        template: template
      },
      elements: [
        {
          tag: 'div',
          text: {
            tag: 'lark_md',
            content: content
          }
        },
        { tag: 'hr' },
        {
          tag: 'action',
          actions: buttonElements
        }
      ]
    }
  };
}

function buildTextMessage(text) {
  return text;
}

module.exports = {
  buildRichTextCard,
  buildInteractiveCard,
  buildTextMessage
};