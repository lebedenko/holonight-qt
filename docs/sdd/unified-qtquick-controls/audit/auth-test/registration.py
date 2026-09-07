"""Correlate this agent's GLib D-Bus registration request and successful reply."""
import re


def registration_reply(log, session):
    blocks = log.split('========================================================================')
    calls = [b for b in blocks if re.search(
        r"member -> 'RegisterAuthenticationAgent(?:WithOptions)?'", b)
        and '>>>> SENT' in b and re.search(r'Type:\s+method-call', b)]
    if len(calls) != 1:
        raise ValueError('Expected exactly one registration request; retry later or use a fresh run.')
    call = calls[0]
    target = re.search(r"destination -> '([^']+)'", call)
    if ("interface -> 'org.freedesktop.PolicyKit1.Authority'" not in call
            or not target
            or "path -> objectpath '/org/freedesktop/PolicyKit1/Authority'" not in call
            or "'unix-session'" not in call
            or f"'session-id': <'{session}'>" not in call
            or "'/org/hyprland/PolicyKit1/AuthenticationAgent'" not in call):
        raise ValueError('Registration does not target the recorded session and expected endpoint.')
    serial = re.search(r'^  Serial:\s+(\d+)\s*$', call, re.M)
    if not serial:
        raise ValueError('Missing registration serial.')
    replies = [b for b in blocks if '<<<< RECEIVED' in b and re.search(
        rf'reply-serial -> uint32 {serial[1]}\s*$', b, re.M)]
    if len(replies) != 1 or not re.search(r'Type:\s+method-return', replies[0]):
        raise ValueError('No unique successful registration reply; pending/error is not a pass.')
    reply = replies[0]
    sender = re.search(r"sender -> '(:[0-9.]+)'", reply)
    destination = re.search(r"destination -> '(:[0-9.]+)'", reply)
    if not sender or not destination or not re.search(r'^  Body: \(\)\s*$', reply, re.M):
        raise ValueError('Unexpected registration reply envelope.')
    if target[1] not in ('org.freedesktop.PolicyKit1', sender[1]):
        raise ValueError('Registration request target differs from the replying authority.')
    if "member -> 'UnregisterAuthenticationAgent'" in log:
        raise ValueError('Agent has unregistered; start a fresh run.')
    return dict(serial=int(serial[1]), authority=sender[1], agent_bus=destination[1])
